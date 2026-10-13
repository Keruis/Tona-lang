add_subdirectory(support)
add_subdirectory(basic)
add_subdirectory(lex)

add_library(tb
  tb.cpp
)

target_link_libraries(tb INTERFACE
  tb_lex
)

target_include_directories(tb PUBLIC
  "${PROJECT_SOURCE_DIR}/include/tb"
)

target_compile_features(tb PUBLIC cxx_std_23)