cmake_minimum_required(VERSION 3.20)
project(miniDB VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
  add_compile_options(/W4 /permissive-)
else()
  add_compile_options(-Wall -Wextra -Wpedantic -Wconversion)
endif()

add_library(minidb_core
  src/bplustree.cpp
  src/table.cpp
  src/storage.cpp
  src/parser.cpp
  src/transaction.cpp
  src/engine.cpp
)

target_include_directories(minidb_core PUBLIC include)

target_compile_features(minidb_core PUBLIC cxx_std_20)

add_executable(minidb_cli src/main.cpp)
target_link_libraries(minidb_cli PRIVATE minidb_core)

enable_testing()
add_executable(minidb_tests tests/minidb_tests.cpp)
target_link_libraries(minidb_tests PRIVATE minidb_core)
add_test(NAME minidb_tests COMMAND minidb_tests)
