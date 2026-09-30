cmake_minimum_required(VERSION 3.16)

project(
    EnglishMentorBot
    VERSION 2.0.0
    DESCRIPTION "Telegram bot for learning English with curated courses and AI practice"
    LANGUAGES CXX
)

include(GNUInstallDirs)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(ENGLISH_MENTOR_BUILD_TESTS "Build unit tests" ON)
option(ENGLISH_MENTOR_WARNINGS_AS_ERRORS "Treat compiler warnings as errors" OFF)
option(ENGLISH_MENTOR_ENABLE_SANITIZERS "Enable AddressSanitizer and UBSan" OFF)

find_package(CURL REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(Threads REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(PQXX REQUIRED IMPORTED_TARGET libpqxx)

add_library(english_mentor_core
    src/app/bot_application.cpp
    src/app/callback_router.cpp
    src/app/command_line.cpp
    src/app/maintenance_commands.cpp
    src/app/message_router.cpp
    src/app/self_test.cpp
    src/ai/groq_client.cpp
    src/ai/backfill_response_parser.cpp
    src/ai/generated_word_parser.cpp
    src/ai/response_fields.cpp
    src/bot/telegram_client_core.cpp
    src/bot/telegram_keyboard.cpp
    src/bot/telegram_client_media.cpp
    src/bot/telegram_client_messages.cpp
    src/bot/telegram_client_support.cpp
    src/bot/telegram_client_transport.cpp
    src/core/text.cpp
    src/presentation/bot_state.cpp
    src/presentation/bot_state_persistence.cpp
    src/presentation/message_cleanup.cpp
    src/presentation/screen_components.cpp
    src/presentation/screen_presentation.cpp
    src/presentation/screen_transport.cpp
    src/presentation/word_presentation.cpp
    src/rendering/cache_maintenance.cpp
    src/rendering/render_cache.cpp
    src/rendering/render_preview.cpp
    src/rendering/html_image_renderer.cpp
    src/rendering/screen_renderer.cpp
    src/rendering/template_engine.cpp
    src/rendering/template_resources.cpp
    src/rendering/words_renderer.cpp
    src/services/broadcast_service.cpp
    src/services/course_catalog.cpp
    src/storage/database_courses.cpp
    src/storage/database_preferences.cpp
    src/presentation/reminder_settings.cpp
    src/services/dictionary_export.cpp
    src/services/dictionary_service.cpp
    src/services/documentation_service.cpp
    src/services/maintenance_service.cpp
    src/services/fallback_dictionary.cpp
    src/services/vocabulary_backfill.cpp
    src/services/vocabulary_policy.cpp
    src/services/vocabulary_presentation.cpp
    src/services/vocabulary_service.cpp
    src/scheduler/broadcast_scheduler.cpp
    src/storage/database_connection.cpp
    src/storage/database_conversations.cpp
    src/storage/database_migrations.cpp
    src/storage/database_users.cpp
    src/storage/database_word_backfill.cpp
    src/storage/database_word_maintenance.cpp
    src/storage/database_word_commands.cpp
    src/storage/database_word_queries.cpp
    src/storage/broadcast_run_state.cpp
    src/storage/runtime_state_paths.cpp
    src/storage/telegram_update_state.cpp
    src/utils/config.cpp
    src/utils/logger.cpp
    src/utils/file_utils.cpp
    src/utils/user_config.cpp
)

target_compile_features(english_mentor_core PUBLIC cxx_std_17)
target_include_directories(
    english_mentor_core
    PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
)
target_link_libraries(
    english_mentor_core
    PUBLIC
        CURL::libcurl
        OpenSSL::Crypto
        PkgConfig::PQXX
        Threads::Threads
)
target_compile_definitions(
    english_mentor_core
    PRIVATE NLOHMANN_JSON_NAMESPACE=nlohmann
)

function(english_mentor_enable_warnings target)
    if(MSVC)
        target_compile_options(${target} PRIVATE /W4)
        if(ENGLISH_MENTOR_WARNINGS_AS_ERRORS)
            target_compile_options(${target} PRIVATE /WX)
        endif()
    else()
        target_compile_options(
            ${target}
            PRIVATE
                -Wall
                -Wextra
                -Wpedantic
                -Wconversion
                -Wshadow
        )
        if(ENGLISH_MENTOR_WARNINGS_AS_ERRORS)
            target_compile_options(${target} PRIVATE -Werror)
        endif()
    endif()
endfunction()

function(english_mentor_enable_sanitizers target)
    if(NOT ENGLISH_MENTOR_ENABLE_SANITIZERS)
        return()
    endif()
    if(MSVC)
        message(FATAL_ERROR "Sanitizer preset is supported with GCC or Clang")
    endif()

    target_compile_options(
        ${target}
        PRIVATE
            -fsanitize=address,undefined
            -fno-omit-frame-pointer
    )
    target_link_options(
        ${target}
        PRIVATE
            -fsanitize=address,undefined
    )
endfunction()

english_mentor_enable_warnings(english_mentor_core)
english_mentor_enable_sanitizers(english_mentor_core)

add_executable(english_mentor src/main.cpp)
target_link_libraries(english_mentor PRIVATE english_mentor_core)
english_mentor_enable_warnings(english_mentor)
english_mentor_enable_sanitizers(english_mentor)

if(ENGLISH_MENTOR_BUILD_TESTS)
    include(CTest)
    enable_testing()

    add_executable(
        english_mentor_tests
        tests/test_main.cpp
    )
    target_link_libraries(english_mentor_tests PRIVATE english_mentor_core)
    english_mentor_enable_warnings(english_mentor_tests)
    english_mentor_enable_sanitizers(english_mentor_tests)

    add_executable(english_mentor_database_tests tests/test_courses_database.cpp)
    target_link_libraries(english_mentor_database_tests PRIVATE english_mentor_core)
    english_mentor_enable_warnings(english_mentor_database_tests)
    english_mentor_enable_sanitizers(english_mentor_database_tests)
    add_test(NAME english_mentor_database_tests COMMAND english_mentor_database_tests)
    set_tests_properties(english_mentor_database_tests PROPERTIES
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}" SKIP_RETURN_CODE 77)
    add_test(NAME english_mentor_unit_tests COMMAND english_mentor_tests)
    add_executable(english_mentor_photo_cache_tests tests/test_photo_cache.cpp
        src/bot/telegram_client_media.cpp src/bot/telegram_client_support.cpp
        src/bot/telegram_keyboard.cpp src/rendering/cache_maintenance.cpp
        src/utils/file_utils.cpp src/utils/logger.cpp)
    target_include_directories(english_mentor_photo_cache_tests PRIVATE include src/bot)
    target_link_libraries(english_mentor_photo_cache_tests PRIVATE OpenSSL::Crypto Threads::Threads)
    english_mentor_enable_warnings(english_mentor_photo_cache_tests)
    add_test(NAME english_mentor_photo_cache_tests COMMAND english_mentor_photo_cache_tests)
    add_executable(english_mentor_reminder_transport_tests
        tests/test_reminder_transport.cpp src/presentation/screen_transport.cpp src/utils/logger.cpp)
    target_include_directories(english_mentor_reminder_transport_tests PRIVATE include)
    english_mentor_enable_warnings(english_mentor_reminder_transport_tests)
    english_mentor_enable_sanitizers(english_mentor_reminder_transport_tests)
    add_test(NAME english_mentor_reminder_transport_tests COMMAND english_mentor_reminder_transport_tests)
    add_executable(english_mentor_export_cleanup_tests tests/test_export_cleanup.cpp
        src/presentation/message_cleanup.cpp src/presentation/bot_state.cpp
        src/presentation/bot_state_persistence.cpp src/utils/file_utils.cpp src/utils/logger.cpp)
    target_include_directories(english_mentor_export_cleanup_tests PRIVATE include src/presentation)
    target_link_libraries(english_mentor_export_cleanup_tests PRIVATE Threads::Threads)
    english_mentor_enable_warnings(english_mentor_export_cleanup_tests)
    english_mentor_enable_sanitizers(english_mentor_export_cleanup_tests)
    add_test(NAME english_mentor_export_cleanup_tests COMMAND english_mentor_export_cleanup_tests)
    find_program(WKHTMLTOPDF_EXECUTABLE wkhtmltopdf)
    if(WKHTMLTOPDF_EXECUTABLE)
        add_executable(english_mentor_export_tests tests/test_dictionary_export.cpp)
        target_link_libraries(english_mentor_export_tests PRIVATE english_mentor_core)
        english_mentor_enable_warnings(english_mentor_export_tests)
        english_mentor_enable_sanitizers(english_mentor_export_tests)
        add_test(NAME english_mentor_export_tests COMMAND english_mentor_export_tests)
        set_tests_properties(english_mentor_export_tests PROPERTIES
            WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
    endif()
    find_program(WKHTMLTOIMAGE_EXECUTABLE wkhtmltoimage)
    if(WKHTMLTOIMAGE_EXECUTABLE)
        add_executable(english_mentor_render_tests tests/test_render_photo_size.cpp)
        target_link_libraries(english_mentor_render_tests PRIVATE english_mentor_core)
        english_mentor_enable_warnings(english_mentor_render_tests)
        english_mentor_enable_sanitizers(english_mentor_render_tests)
        add_test(NAME english_mentor_render_tests COMMAND english_mentor_render_tests)
    endif()

    set_tests_properties(
        english_mentor_unit_tests
        PROPERTIES WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    )
endif()

install(
    TARGETS english_mentor
    RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
)
install(
    DIRECTORY resources migrations
    DESTINATION "${CMAKE_INSTALL_DATADIR}/english_mentor"
)
