const std = @import("std");

const targets: []const std.Target.Query = &.{
  .{.cpu_arch = .x86_64, .os_tag = .linux, .abi = .gnu}
};

pub fn build(b: *std.Build) !void {
  const target = b.standardTargetOptions(.{});

  const token_c = b.addTranslateC(.{
    .root_source_file = b.path("src/tona.h"),
    .target = target,
    .optimize = .Debug,
  });

  const tona_exe = b.addExecutable(.{
    .name = "tona",
    .root_module = b.createModule(.{
      .root_source_file = b.path("tona/tona.zig"),
      .target = target,
      .optimize = .Debug,
      .imports = &.{
        .{
          .name = "tona_h",
          .module = token_c.createModule()
        }
      },
      .link_libc = true
    }),
  });

  const tona_module = b.createModule(.{
    .root_source_file = b.path("src/root.zig"),
    .target = target
  });

  tona_exe.root_module.addObjectFile(b.path("build/libtona.a"));
  tona_exe.root_module.addImport("tona", tona_module);
  tona_exe.root_module.addObjectFile(.{
    .cwd_relative = "/usr/lib/libstdc++.a"
  });
  tona_exe.root_module.linkSystemLibrary("gcc_s", .{});

  b.installArtifact(tona_exe);

  const run_exe = b.addRunArtifact(tona_exe);

  run_exe.addArgs(&.{
    "-i"
  });

  const run_step = b.step("run", "Run the application");
  run_step.dependOn(&run_exe.step);
}
