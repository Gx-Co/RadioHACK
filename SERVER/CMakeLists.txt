cmake_minimum_required(VERSION 3.16)
project(RadioHACK)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Поиск зависимостей
find_package(PkgConfig REQUIRED)
pkg_check_modules(SQLITE3 REQUIRED sqlite3)
find_package(Threads REQUIRED)

# Включение директорий
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/third_party)

# Источники
set(SOURCES
    src/main.cpp
    src/server.cpp
    src/database.cpp
    src/auth.cpp
    src/utils.cpp
    src/config.cpp
)

# Добавление исполняемого файла
add_executable(RadioHACK ${SOURCES})

# Подключение библиотек
target_link_libraries(RadioHACK 
    ${SQLITE3_LIBRARIES}
    Threads::Threads
    ssl crypto
)

# Флаги компиляции
target_compile_options(RadioHACK PRIVATE -Wall -Wextra)
target_include_directories(RadioHACK PRIVATE ${SQLITE3_INCLUDE_DIRS})