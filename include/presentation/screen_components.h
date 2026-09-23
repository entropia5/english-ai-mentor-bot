#pragma once

#include "domain/reminder_settings.h"
#include "presentation/inline_keyboard.h"

#include <initializer_list>
#include <string>
#include <utility>

InlineKeyboard column_keyboard(std::initializer_list<std::pair<std::string, std::string>> buttons);
InlineKeyboard main_menu_keyboard();
InlineKeyboard topic_keyboard();
InlineKeyboard page_keyboard(const std::string& callback_prefix, int page, int total,
                             const std::string& menu_callback = "menu_main");
std::string screen_caption(const std::string& title, int page = -1, int total = -1,
                           const std::string& hint = "");

InlineKeyboard course_filter_keyboard(const std::string& prefix, const std::string& selected);
InlineKeyboard reminder_settings_keyboard(const ReminderSettings& settings);
