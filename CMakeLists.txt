cmake_minimum_required(VERSION 4.3.0)

set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "451f2fe2-a8a2-47c3-bc32-94786d8fc91b")

project(Tona LANGUAGES CXX)

set(CMAKE_CXX_MODULE_STD 1)
set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address -fsanitize=undefined -fno-omit-frame-pointer")

file(GLOB_RECURSE ALL_IXX_FILES CONFIGURE_DEPENDS 
  "src/*.ixx"
)

add_executable(tona
  tona/tona.cpp
)

target_sources(tona
  PUBLIC
  FILE_SET CXX_MODULES FILES
  tona/telysia.ixx
  ${ALL_IXX_FILES}
)

target_compile_options(tona PRIVATE
  -save-temps=obj
  -fverbose-asm
)