ALTER TABLE words
    ADD COLUMN IF NOT EXISTS definition_ru TEXT;

WITH ranked AS (
    SELECT
        id,
        ROW_NUMBER() OVER (
            PARTITION BY user_id, lower(trim(english))
            ORDER BY is_learned DESC, added_date ASC, id ASC
        ) AS row_number
    FROM words
)
DELETE FROM words
USING ranked
WHERE words.id = ranked.id
  AND ranked.row_number > 1;

CREATE UNIQUE INDEX IF NOT EXISTS idx_words_user_english_unique
    ON words(user_id, lower(trim(english)));
