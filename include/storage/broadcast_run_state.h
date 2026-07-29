#pragma once

#include <ctime>
#include <string>

std::string local_date_key(const std::tm& time);
bool broadcast_was_sent(const std::string& date_key, const std::string& kind, long long user_id);
void remember_broadcast_sent(const std::string& date_key, const std::string& kind,
                             long long user_id);
