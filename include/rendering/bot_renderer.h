#pragma once

#include "database.h"

#include <string>
#include <vector>

bool cleanup_render_cache();

std::string render_main_menu_image(const std::string& user_level);
std::string render_ai_prompt_image();
std::string render_reminder_settings_image();
std::string render_topic_menu_image();
std::string render_dictionary_export_image();
std::string render_status_image(const std::string& key, const std::string& title,
                                const std::string& subtitle, const std::string& note);
std::string render_stats_image(long long chat_id, int total, int learned, const std::string& level,
                               const std::string& next_name, int next_level, int percent);
std::string render_dictionary_words_image(long long chat_id, const std::vector<WordView>& words,
                                          int page, int total, int start, int end,
                                          const std::string& filter = "all");
std::string render_learned_words_image(long long chat_id, const std::vector<WordView>& words,
                                       int page, int total, int start, int end,
                                       const std::string& filter = "all");
std::string render_daily_review_image(long long chat_id, const std::vector<WordView>& words,
                                      int page, int total, int start, int end,
                                      const std::string& filter = "all");
std::string render_evening_words_image(long long chat_id, const std::vector<WordView>& words,
                                       int page, int total, int start, int end,
                                       const std::string& filter = "all");
std::string format_word(const std::string& english, const std::string& translation,
                        const std::string& transcription, const std::string& pronunciation,
                        const std::string& definition);
