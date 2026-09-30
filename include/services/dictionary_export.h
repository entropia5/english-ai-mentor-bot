#pragma once
#include "domain/word.h"
#include <filesystem>
#include <string>
#include <vector>
class Database;
class TelegramClient;
enum class DictionaryExportKind { Learned, Conversation };
std::vector<Word> conversation_words_for_export();
std::string dictionary_export_html(const std::vector<Word>& words,
    DictionaryExportKind kind = DictionaryExportKind::Learned);
void render_dictionary_pdf(const std::vector<Word>& words, const std::filesystem::path& directory,
    DictionaryExportKind kind = DictionaryExportKind::Learned);
void show_dictionary_export_menu(long long chat_id, TelegramClient& bot, int message_id = 0);
void send_dictionary_pdf(long long chat_id, TelegramClient& bot, Database& database,
                         DictionaryExportKind kind);
