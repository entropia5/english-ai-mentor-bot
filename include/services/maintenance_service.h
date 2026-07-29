#pragma once

class Database;

bool audit_bad_words(Database& database, bool remove_words);
