add_library(tb_lex
  Lexer.cpp
  LiteralSupport.cpp
)

target_link_libraries(tb_lex PRIVATE
  tb_basic
  tb_support
)

target_include_directories(tb_lex PUBLIC
  "${PROJECT_SOURCE_DIR}/include/tb"
)

target_compile_features(tb_lex PUBLIC cxx_std_23)