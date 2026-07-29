
#include "app/bot_application.h"
#include "app/command_line.h"
#include "app/maintenance_commands.h"
#include "config.h"
#include "database.h"
#include "groq_client.h"
#include "logger.h"
#include "presentation/bot_state.h"
#include "scheduler/broadcast_scheduler.h"
#include "services/broadcast_service.h"
#include "telegram_client.h"
#include "user_config.h"

#include <iostream>

int main(int argc, char* argv[]) {
    std::cout << "=== English Mentor Bot v2 ===" << std::endl;

    const CommandLineParseResult command_line = parse_command_line(argc, argv);
    if (!command_line.ok()) {
        LOG_ERROR(command_line.error);
        return 1;
    }
    const AppOptions& options = command_line.options;

    const app::detail::CommandResult pre_config = app::detail::run_pre_config_command(options);
    if (pre_config.handled) {
        return pre_config.exit_code;
    }

    if (!g_config.load(".env") && !g_config.load("../.env")) {
        LOG_ERROR("Failed to load .env");
        return 1;
    }

    const app::detail::CommandResult maintenance = app::detail::run_database_maintenance(options);
    if (maintenance.handled) {
        return maintenance.exit_code;
    }

    std::string token = g_config.get("TELEGRAM_TOKEN");
    if (token.empty()) {
        LOG_ERROR("TELEGRAM_TOKEN not found");
        return 1;
    }

    TelegramClient bot(token);
    if (!bot.test_token()) {
        LOG_ERROR("Invalid token");
        return 1;
    }
    bot.delete_webhook(false);

    Database db;
    db.connect();
    db.init_tables();

    load_bot_state();

    if (options.send_evening_once) {
        long long target_chat_id =
            options.evening_chat_id > 0 ? options.evening_chat_id : configured_user_id("USER_1_ID");
        if (target_chat_id <= 0) {
            LOG_ERROR("--send-evening-once requires a chat id or USER_1_ID in .env");
            return 1;
        }

        BroadcastResult result = send_evening_new_words(target_chat_id, bot, db);
        return result == BroadcastResult::Failed ? 1 : 0;
    }

    GroqClient ai;
    LOG("Bot started!");

    BroadcastScheduler scheduler(bot);
    scheduler.start();
    LOG("Scheduler thread launched");

    return run_bot_application(bot, db, ai);
}
