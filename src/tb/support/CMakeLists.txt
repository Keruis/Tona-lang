add_library(tb_support
  UTF8.cpp
)

target_include_directories(tb_support PUBLIC
  "${PROJECT_SOURCE_DIR}/include"
  "${PROJECT_SOURCE_DIR}/include/tb"
)

target_compile_features(tb_support PUBLIC cxx_std_23)