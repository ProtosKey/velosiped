cmake_minimum_required(VERSION 3.21)
project(vls C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

find_package(OpenSSL REQUIRED)

add_executable(vls
    src/main.c
    src/vls_command.c
    src/utils/console_writer.c
    src/utils/vls_hasher.c
    src/commands/init.c
    src/commands/add.c
    src/commands/commit.c
    src/commands/reset.c
    src/commands/commit.c
)

target_include_directories(vls PRIVATE include)
target_link_libraries(vls PRIVATE OpenSSL::Crypto)
