#pragma once

class Database;
class GroqClient;
class TelegramClient;

int run_bot_application(TelegramClient& bot, Database& database, GroqClient& ai);
