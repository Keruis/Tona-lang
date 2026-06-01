cmake_minimum_required(VERSION 4.3.0)

set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "451f2fe2-a8a2-47c3-bc32-94786d8fc91b")

project(Tona-lib LANGUAGES CXX)

set(CMAKE_CXX_MODULE_STD 1)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(tona STATIC
  elysia.ixx
)

target_sources(tona
  PUBLIC
  FILE_SET CXX_MODULES FILES
  elysia.ixx
  lexer/lexer.ixx
  lexer/token.ixx
)