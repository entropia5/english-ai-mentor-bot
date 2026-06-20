cmake_minimum_required(VERSION 3.10)
project(EnglishMentorBot)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# search for libraries
find_package(CURL REQUIRED)
find_package(Threads REQUIRED)
find_package(PkgConfig REQUIRED)

#  libpqxx
pkg_check_modules(PQXX REQUIRED libpqxx)

# pwd for includes
include_directories(include)
include_directories(${CURL_INCLUDE_DIRS})
include_directories(${PQXX_INCLUDE_DIRS})

# source files
set(SOURCES
    src/main.cpp
    src/utils/config.cpp
    src/utils/logger.cpp
    src/utils/render_utils.cpp
    src/storage/database.cpp
    src/bot/telegram_client.cpp
    src/ai/groq_client.cpp
)
# build executable
add_executable(english_mentor ${SOURCES})

# link libraries
target_link_libraries(english_mentor
    ${CURL_LIBRARIES}
    ${PQXX_LIBRARIES}
    Threads::Threads
)

# work with json
target_compile_definitions(english_mentor PRIVATE -DNLOHMANN_JSON_NAMESPACE=nlohmann)

# output build configuration
message(STATUS "=== Build Configuration ===")
message(STATUS "CURL: ${CURL_LIBRARIES}")
message(STATUS "PQXX: ${PQXX_LIBRARIES}")
message(STATUS "==========================")
