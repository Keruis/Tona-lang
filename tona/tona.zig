const std = @import("std");
const tona = @import("tona");

pub fn main(init: std.process.Init) void {
  
  const lexer = tona.Lexer.init();
  defer lexer.deinit();

  const ctx = lexer.scan_tokens();
  defer ctx.deinit();

  if (init.minimal.args.vector.len < 2)
    std.debug.print("Args < 2\n", .{});

  var args_iter = init.minimal.args.iterate();
  _ = args_iter.skip();
  
  while (args_iter.next()) |arg| {
    _ = arg;
  }
}


