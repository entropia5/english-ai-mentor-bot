CREATE TABLE IF NOT EXISTS users (
    user_id BIGINT PRIMARY KEY,
    name VARCHAR(100),
    level INTEGER DEFAULT 1,
    streak_days INTEGER DEFAULT 0,
    last_active BIGINT DEFAULT 0,
    last_daily_sent BIGINT DEFAULT 0,
    new_words_page INTEGER DEFAULT 0,
    learned_words_page INTEGER DEFAULT 0,
    last_viewed_dict VARCHAR(10) DEFAULT 'new',
    created_at BIGINT DEFAULT EXTRACT(EPOCH FROM NOW())
);

CREATE TABLE IF NOT EXISTS words (
    id SERIAL PRIMARY KEY,
    user_id BIGINT REFERENCES users(user_id) ON DELETE CASCADE,
    english VARCHAR(255) NOT NULL,
    transcription VARCHAR(255),
    pronunciation_ru VARCHAR(255),
    translation_ru TEXT,
    definition_ru TEXT,
    repetition_count INTEGER DEFAULT 0,
    last_repetition BIGINT DEFAULT 0,
    next_review BIGINT DEFAULT 0,
    is_learned BOOLEAN DEFAULT FALSE,
    level INTEGER DEFAULT 1,
    topic VARCHAR(100) DEFAULT 'general',
    added_date BIGINT DEFAULT EXTRACT(EPOCH FROM NOW()),
    correct_in_row INTEGER DEFAULT 0
);

CREATE TABLE IF NOT EXISTS conversations (
    id SERIAL PRIMARY KEY,
    user_id BIGINT REFERENCES users(user_id) ON DELETE CASCADE,
    role VARCHAR(10),
    content TEXT,
    timestamp BIGINT DEFAULT EXTRACT(EPOCH FROM NOW())
);

CREATE INDEX IF NOT EXISTS idx_words_user_learned
    ON words(user_id, is_learned);

CREATE INDEX IF NOT EXISTS idx_words_next_review
    ON words(next_review);

CREATE INDEX IF NOT EXISTS idx_words_user_english_lower
    ON words(user_id, lower(english));

CREATE INDEX IF NOT EXISTS idx_conversations_user_time
    ON conversations(user_id, timestamp);
