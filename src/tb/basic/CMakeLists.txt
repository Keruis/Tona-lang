add_library(tb_basic
  Diagnostic.cpp
  Source.cpp
)

target_include_directories(tb_basic PUBLIC
  "${PROJECT_SOURCE_DIR}/include/tb"
)

target_compile_features(tb_basic PUBLIC cxx_std_23)