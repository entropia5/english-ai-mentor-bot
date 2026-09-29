-- Подборка и новые слова сохраняются вместе: повтор отправки не добавляет лишних слов.
CREATE TABLE evening_word_batches (
    user_id BIGINT NOT NULL REFERENCES users(user_id) ON DELETE CASCADE,
    batch_date DATE NOT NULL,
    words JSONB NOT NULL,
    PRIMARY KEY (user_id, batch_date)
);
