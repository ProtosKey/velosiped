cmake_minimum_required(VERSION 3.21)
project(vls C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

enable_testing()

find_package(OpenSSL REQUIRED)
find_package(cJSON REQUIRED)

add_library(unity STATIC tests/third_party/unity/unity.c)
target_include_directories(unity PUBLIC tests/third_party/unity)


add_library(vls_core STATIC
  src/vls_command.c
  src/utils/input_output.c
  src/utils/visitor.c
  src/utils/logger.c
  src/utils/hasher.c
  src/utils/stager.c
  src/utils/iterator.c
  src/utils/fs.c
  src/commands/init.c
  src/commands/add.c
  src/commands/commit.c
  src/commands/help.c
  src/commands/reset.c
  src/commands/status.c)


target_include_directories(vls_core PUBLIC include)
target_compile_definitions(vls_core PUBLIC _POSIX_C_SOURCE=200809L _XOPEN_SOURCE=700)
target_link_libraries(vls_core PUBLIC OpenSSL::Crypto cjson)

add_executable(vls
    src/main.c
)
target_link_libraries(vls PRIVATE vls_core)

function(add_unit_test name)
  add_executable(${name} tests/unit/${name}.c)
  target_link_libraries(${name} PRIVATE unity vls_core)
  add_test(NAME ${name} COMMAND ${name})
endfunction()

add_unit_test(test_fs)
add_unit_test(test_hasher)

add_test(
    NAME smoke
    COMMAND ${CMAKE_COMMAND} -E env VLS_BIN=$<TARGET_FILE:vls>
            bash ${CMAKE_SOURCE_DIR}/tests/integration/smoke.sh
)
