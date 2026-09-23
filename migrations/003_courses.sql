ALTER TABLE users ADD COLUMN IF NOT EXISTS active_course VARCHAR(32) NOT NULL DEFAULT 'conversation';

-- Existing vocabulary is retained; a full reset is a separate, explicit maintenance action.
UPDATE words SET topic = CASE WHEN topic IN ('IT programming', 'it') THEN 'it'
                             WHEN topic = 'medicine' THEN 'medicine'
                             ELSE 'conversation' END;
DROP INDEX IF EXISTS idx_words_user_english_unique;
CREATE UNIQUE INDEX IF NOT EXISTS idx_words_user_course_english_unique
    ON words(user_id, topic, lower(trim(english)));
