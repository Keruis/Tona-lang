add_library(tona
  main.cpp
)

target_include_directories(tona PUBLIC
  "${PROJECT_SOURCE_DIR}/include/tona"
)

target_link_libraries(tona PRIVATE tb)

target_compile_features(tona PUBLIC cxx_std_23)