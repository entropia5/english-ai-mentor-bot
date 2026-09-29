#include "scheduler/broadcast_scheduler.h"

#include "database.h"
#include "rendering/cache_maintenance.h"
#include "logger.h"
#include "presentation/bot_presentation.h"
#include "services/broadcast_service.h"
#include "storage/broadcast_run_state.h"
#include "telegram_client.h"

#include <chrono>
#include <ctime>

BroadcastScheduler::BroadcastScheduler(TelegramClient& bot) : bot_(bot) {}

BroadcastScheduler::~BroadcastScheduler() {
    stop();
}

void BroadcastScheduler::start() {
    if (worker_.joinable()) {
        return;
    }
    stopping_.store(false);
    worker_ = std::thread(&BroadcastScheduler::run, this);
}

void BroadcastScheduler::stop() {
    stopping_.store(true);
    wake_up_.notify_all();
    if (worker_.joinable()) {
        worker_.join();
    }
}

bool BroadcastScheduler::wait_for_stop(std::chrono::seconds duration) {
    std::unique_lock<std::mutex> lock(wait_mutex_);
    return wake_up_.wait_for(lock, duration, [this] { return stopping_.load(); });
}

void BroadcastScheduler::run() {
    LOG("Scheduler thread started");
    Database scheduler_database;

    auto ensure_database = [&]() {
        if (scheduler_database.is_connected()) {
            return true;
        }
        if (!scheduler_database.connect()) {
            LOG_ERROR("Scheduler database connection failed");
            return false;
        }
        if (!scheduler_database.init_tables()) {
            LOG_ERROR("Scheduler database initialization failed");
            scheduler_database.disconnect();
            return false;
        }
        return true;
    };

    while (!stopping_.load()) {
        try { maintain_render_cache(); }
        catch (const std::exception& error) { LOG_ERROR(std::string("Cache cleanup: ") + error.what()); }
        const auto now = std::chrono::system_clock::now();
        const std::time_t now_time = std::chrono::system_clock::to_time_t(now);
        const std::tm* now_tm = std::localtime(&now_time);
        if (now_tm == nullptr) {
            LOG_ERROR("Scheduler failed to read local time");
            if (wait_for_stop(std::chrono::seconds(60))) {
                break;
            }
            continue;
        }

        const std::string date_key = local_date_key(*now_tm);
        if (now_tm->tm_hour == 9 && now_tm->tm_min < 5 && ensure_database()) {
            LOG("Checking learned words review at 09:" + std::to_string(now_tm->tm_min));
            for (const long long user_id : configured_broadcast_users()) {
                if (stopping_.load()) {
                    break;
                }
                if (broadcast_was_sent(date_key, "morning", user_id)) {
                    continue;
                }
                const BroadcastResult result = send_daily_review(user_id, bot_, scheduler_database);
                if (result == BroadcastResult::Delivered || result == BroadcastResult::NoContent) {
                    remember_broadcast_sent(date_key, "morning", user_id);
                }
                if (wait_for_stop(std::chrono::seconds(2))) {
                    break;
                }
            }
        }

        if (now_tm->tm_hour == 21 && now_tm->tm_min < 5 && ensure_database()) {
            LOG("Checking evening new words at 21:" + std::to_string(now_tm->tm_min));
            for (const long long user_id : configured_broadcast_users()) {
                if (stopping_.load()) {
                    break;
                }
                if (broadcast_was_sent(date_key, "evening", user_id)) {
                    continue;
                }
                const BroadcastResult result =
                    send_evening_new_words(user_id, bot_, scheduler_database);
                if (result == BroadcastResult::Delivered || result == BroadcastResult::NoContent) {
                    remember_broadcast_sent(date_key, "evening", user_id);
                }
                if (wait_for_stop(std::chrono::seconds(2))) {
                    break;
                }
            }
        }

        wait_for_stop(std::chrono::seconds(60));
    }
    LOG("Scheduler thread stopped");
}
