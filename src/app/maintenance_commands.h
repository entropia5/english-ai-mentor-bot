#pragma once

struct AppOptions;

namespace app::detail {

struct CommandResult {
    bool handled = false;
    int exit_code = 0;
};

CommandResult run_pre_config_command(const AppOptions& options);
CommandResult run_database_maintenance(const AppOptions& options);

} // namespace app::detail
