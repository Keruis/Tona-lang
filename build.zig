const std = @import("std");

const targets: []const std.Target.Query = &.{
  .{.cpu_arch = .x86_64, .os_tag = .linux, .abi = .gnu}
};

pub fn build(b: *std.Build) !void {
  const target = b.standardTargetOptions(.{});

  const tona_c = b.addTranslateC(.{
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
      .link_libc = true
    }),
  });

  const tona_module = b.createModule(.{
    .root_source_file = b.path("src/root.zig"),
    .target = target,
    .imports = &.{
      .{
        .name = "tona_c",
        .module = tona_c.createModule()
      }
    },
  });

  tona_module.addObjectFile(b.path("build/libtona.a"));
  tona_module.addObjectFile(.{
    .cwd_relative = "/usr/lib/libstdc++.a"
  });
  tona_module.linkSystemLibrary("gcc_s", .{});

  tona_exe.root_module.addImport("tona", tona_module);

  b.installArtifact(tona_exe);

  const run_exe = b.addRunArtifact(tona_exe);

  run_exe.addArgs(&.{
    "-i"
  });

  const run_step = b.step("run", "Run the application");
  run_step.dependOn(&run_exe.step);
}
