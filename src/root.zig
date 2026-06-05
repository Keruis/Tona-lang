const tona_c = @import("tona_c");

pub const LexerError = error {
  TokenizationFailed
};

pub const TokenError = tona_c.TokenError;

pub const TokenContext = struct {
  extern fn destroy_token_ctx(ctx: *tona_c.TokenContext) void;

  handle: *tona_c.TokenContext,

  pub fn deinit(self: TokenContext) void {
    destroy_token_ctx(self.handle);
  }
};

pub const Lexer = struct {
  extern fn create_lexer() *tona_c.Lexer;
  extern fn destroy_lexer(lex: *tona_c.Lexer) void;
  extern fn tokens(lex: *tona_c.Lexer, err: *tona_c.TokenError) ?*tona_c.TokenContext;

  handle: *tona_c.Lexer,

  pub fn init() Lexer {
    return .{
      .handle = create_lexer()
    };
  }

  pub fn deinit(self: Lexer) void {
    destroy_lexer(self.handle);
  }

  pub fn scan_tokens(self: Lexer, err: *tona_c.TokenError) LexerError!TokenContext {
    const ctx_ptr = tokens(self.handle, err) orelse return error.TokenizationFailed;
    return .{
      .handle = ctx_ptr
    };
  }
};