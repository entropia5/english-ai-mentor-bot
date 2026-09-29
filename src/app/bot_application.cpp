#include "app/bot_application.h"

#include "bot_application_internal.h"
#include "logger.h"
#include "presentation/bot_state.h"
#include "storage/telegram_update_state.h"
#include "telegram_client.h"

#include <chrono>
#include <atomic>
#include <nlohmann/json.hpp>
#include <string>
#include <thread>

namespace {
// Удаляем сообщения независимо от долгого опроса Telegram и отрисовки карточек.
class MessageCleanupWorker {
  public:
    explicit MessageCleanupWorker(TelegramClient& bot) : worker_([this, &bot] {
        while (!stopping_.load()) {
            try {
                process_deferred_message_deletions(bot);
            } catch (const std::exception& error) {
                LOG_ERROR(std::string("Message cleanup: ") + error.what());
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
    }) {}
    ~MessageCleanupWorker() {
        stopping_.store(true);
        worker_.join();
    }
  private:
    std::atomic<bool> stopping_{false};
    std::thread worker_;
};
}

int run_bot_application(TelegramClient& bot, Database& database, GroqClient& ai) {
    MessageCleanupWorker cleanup(bot);
    int last_id = load_update_offset();
    LOG("Starting Telegram polling from update offset " + std::to_string(last_id + 1));
    app::detail::SessionState session;

    while (true) {
        try {
            const std::string updates_text = bot.get_updates(last_id + 1, 10);
            if (updates_text.empty() || updates_text == "{}") {
                continue;
            }

            const auto updates = nlohmann::json::parse(updates_text);
            if (!updates.value("ok", false)) {
                LOG_ERROR("getUpdates API failed: " +
                          updates.value("description", "unknown error"));
                std::this_thread::sleep_for(std::chrono::seconds(2));
                continue;
            }

            for (const auto& update : updates["result"]) {
                last_id = update["update_id"];
                save_update_offset(last_id);

                if (update.contains("callback_query")) {
                    app::detail::handle_callback(update, bot, database, ai, session);
                    continue;
                }
                if (update.contains("message") && update["message"].contains("text")) {
                    app::detail::handle_text_message(update, bot, database, ai, session);
                }
            }
        } catch (const std::exception& error) {
            LOG_ERROR(std::string(error.what()));
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    return 0;
}
