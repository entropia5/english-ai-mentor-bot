ALTER TABLE users
    ADD COLUMN dictionary_filter VARCHAR(32) NOT NULL DEFAULT 'all',
    ADD COLUMN morning_enabled BOOLEAN NOT NULL DEFAULT TRUE,
    ADD COLUMN evening_enabled BOOLEAN NOT NULL DEFAULT TRUE,
    ADD COLUMN morning_course VARCHAR(32) NOT NULL DEFAULT 'all',
    ADD COLUMN evening_course VARCHAR(32) NOT NULL DEFAULT 'all',
    ADD COLUMN evening_add_new BOOLEAN NOT NULL DEFAULT TRUE;

ALTER TABLE users
    ADD CONSTRAINT valid_dictionary_filter CHECK (dictionary_filter IN ('all','conversation','medicine','it')),
    ADD CONSTRAINT valid_morning_course CHECK (morning_course IN ('all','conversation','medicine','it')),
    ADD CONSTRAINT valid_evening_course CHECK (evening_course IN ('all','conversation','medicine','it'));

-- Keep existing reminder sources on migration; browsing now starts with all courses.
UPDATE users SET morning_course=active_course, evening_course=active_course;
