# Architecture

English Mentor Bot is a modular C++17 application. The executable is deliberately
small: `src/main.cpp` is the composition root that creates infrastructure objects,
wires services, starts the scheduler and hands control to the application loop.

## Dependency direction

```text
                       +------------------+
Telegram Bot API <---- | presentation/bot |
                       +--------+---------+
                                |
                                v
+-------------+       +---------+---------+       +------------------+
| app routing | ----> | application       | ----> | services         |
+-------------+       | use cases         |       +----+--------+----+
                      +-------------------+            |        |
                                                       v        v
                                                 +---------+ +---------+
                                                 | storage | | AI API  |
                                                 +---------+ +---------+
                                                       |
                                                       v
                                                  PostgreSQL
```

Dependencies point from orchestration toward domain and infrastructure interfaces.
The domain model does not know about Telegram, HTTP, JSON or PostgreSQL.

## Modules

- `domain` — named business models (`Word`) and pure rules.
- `core` — small reusable text operations without infrastructure dependencies.
- `services` — application use cases: curated vocabulary selection, dictionary updates,
  broadcasts, documentation export and maintenance. Documentation screenshots are
  isolated in `documentation_service` and are not part of scheduled delivery.
- `ai` — the Groq adapter and layered machine-readable response parsing:
  `response_fields` tokenizes labels, while generated-word and backfill parsers
  assemble their own domain records.
- `app` — command-line parsing and Telegram update orchestration:
  `bot_application` owns only the polling loop, while `callback_router` and
  `message_router` handle their respective update types. One-shot maintenance
  modes are dispatched by `maintenance_commands`, keeping `main.cpp` as the
  composition root. The legacy CLI smoke suite is isolated in `self_test`;
  configured user-ID parsing is shared through `user_config`.
- `presentation` — Telegram screens, pagination and per-chat UI state.
- `rendering` — runtime template loading, view-data preparation and conversion into
  PNG screens. HTML structure and CSS live outside the executable.
- `ai` — Groq client support and validation of structured model responses.
- `bot` — Telegram Bot API adapter.
- `storage` — PostgreSQL access, migrations and durable runtime checkpoints.
- `scheduler` — ownership and lifecycle of scheduled broadcast work.

`english_mentor_core` contains all reusable application logic. Both the production
executable and unit-test executable link against this target, so tests exercise the
same compiled code that runs in production.

## Runtime flow

1. `main` loads and validates configuration.
2. PostgreSQL is connected and pending migrations are applied in order.
3. Telegram, AI, database and service objects are constructed explicitly.
4. The broadcast scheduler starts its owned worker thread.
5. `run_bot_application` polls Telegram from the last durable update offset.
6. The router delegates work to services and presentation functions.
7. Shutdown stops the scheduler through RAII and releases infrastructure objects.

## State and concurrency

Telegram UI state belongs to an internal `BotStateStore`; it is not exposed as a
collection of global containers. Persistent update offsets and broadcast execution
markers have separate contracts in `storage/telegram_update_state` and
`storage/broadcast_run_state`. They share only a private state-directory policy.

Background work does not retain references to short-lived bot objects. Deferred
Telegram message deletions are queued and executed by the main event loop.
`BroadcastScheduler` owns its worker thread and guarantees a safe stop/join during
destruction.

## Persistence

Schema changes are append-only SQL files in `migrations/`. Applied versions are
recorded in `schema_migrations`, making startup idempotent. New migrations must never
rewrite a version that may already have run in another environment.

PostgreSQL method implementations are grouped by responsibility while preserving one
explicit `Database` facade:

- `database_connection` — PostgreSQL connection lifecycle;
- `database_migrations` — migration discovery, ordering and transactional application;
- `database_users` — user profiles and levels;
- `database_conversations` — AI conversation persistence and history;
- `database_word_commands` — per-user vocabulary writes;
- `database_word_queries` — per-user vocabulary read models;
- `database_word_backfill` — queries and updates for missing word metadata;
- `database_word_maintenance` — duplicate cleanup, audits and explicit deletion.

Vocabulary generation is a pipeline of explicit modules:

- `vocabulary_policy` defines acceptable dictionary headwords and blocked candidates;
- `fallback_dictionary` loads, validates and deduplicates runtime JSON vocabulary;
- `vocabulary_backfill` owns maintenance batches for missing pronunciation/definitions;
- `vocabulary_service` orchestrates AI attempts and persistence;
- `vocabulary_presentation` owns the Telegram progress and result screens.

The `data/` directory contains runtime checkpoints and generated image cache. It is
intentionally excluded from Git.

## Runtime rendering

The visual layer lives in `resources/rendering`, not in C++ string literals.
`template_resources` locates the runtime resource directory, loads the selected HTML file and
collects all `styles/*.css` modules. `template_engine` validates placeholders, escapes dynamic
values and produces the final HTML. Styles are concatenated in lexicographic order, so numbered
filenames provide a stable CSS cascade without a rebuild.
Escaped scalar values and explicitly generated safe HTML fragments are inserted into
the template before `wkhtmltoimage` produces a PNG.

The final rendered HTML fingerprint participates in the cache key. Editing a
template or the theme therefore invalidates only the affected output naturally,
without recompiling the executable or manually deleting the cache.
Generic text-file and atomic-write operations live in `file_utils`; the external
`wkhtmltoimage` process and PNG validation are isolated in `html_image_renderer`.

`RENDER_RESOURCES_DIR` can point to an external theme directory. Docker Compose
mounts the repository rendering resources read-only into the bot container, allowing
live visual adjustments.

The Telegram adapter keeps one public `TelegramClient` facade, while implementation
files are split by protocol responsibility:

- `telegram_client_core` — polling, bot identity and webhook lifecycle;
- `telegram_client_messages` — text messages, keyboards, callbacks and deletion;
- `telegram_client_media` — Telegram photo payloads and API result handling;
- `telegram_client_transport` — the shared CURL lifecycle for JSON and multipart POST requests;
- `telegram_keyboard` — inline keyboard serialization;
- `telegram_client_support` — JSON response validation and failure classification
  shared by the other adapter modules.

Presentation delivery is isolated in `screen_transport`: it owns the Telegram
edit-or-replace policy for text and photo dashboards. Pure keyboard construction,
pagination controls and captions live in `screen_components`. Screen view functions
do not implement Telegram failure recovery directly.

Presentation state is isolated in `bot_state`: it owns live dashboard identifiers
and stable screen contexts. `message_cleanup` owns tracked temporary messages,
reply-keyboard cleanup and deferred deletion tasks.
`bot_state_persistence` independently owns the JSON checkpoint format and filesystem
I/O. Both share a private `BotStateStore` model that is not exposed outside the
presentation implementation. Concrete views are split between `screen_presentation`
and `word_presentation`; the latter uses one private pagination policy for every
word-list screen. Consumers include the state API explicitly when they need it.

## Architectural boundaries

When adding functionality:

- keep `main.cpp` limited to dependency construction and lifecycle;
- put business decisions in `domain` or `services`, not Telegram handlers;
- keep SQL inside `storage`;
- keep API-specific JSON inside the relevant adapter;
- keep HTML structure and visual constants in `resources/rendering`;
- prefer explicit dependencies passed by reference over new global state;
- add a unit test for pure parsing, validation and domain behavior.

## Curated courses

`course_catalog` loads versioned JSON and rejects invalid, duplicate or incomplete cards.
Conversation has exactly 2000 entries; professional courses also allow phrases and
acronyms. Selection is deterministic, excludes already learned and pending words, and
stops at the end of the catalog. Groq is not called for course additions.

`users.active_course` selects manual additions, AI practice and course statistics.
`users.dictionary_filter` independently selects all courses or one course for both
dictionary views. Reminder preferences persist independently for morning and evening:
enabled flag, course filter, and evening repeat/add-new mode. Migration 004 preserves
existing reminder sources and vocabulary; dictionary browsing initially shows all.
Reminder preparation is testable without Telegram, and disabled reminders do not add words.
Uniqueness is `(user_id, topic, lower(trim(english)))`, preserving distinct professional
meanings. `database_courses` uses a per-user transaction advisory lock and a single
transaction for batch selection and insertion, preventing duplicate concurrent additions.
Legacy audit commands exclude approved course vocabulary.

Migration 003 preserves existing vocabulary and maps retired topics into courses.
Clearing production words is an explicit one-off operation after a verified backup,
never an automatic startup migration.
