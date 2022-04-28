---
content_title: DCD Manual Build
---

[[info | Manual Builds are for Advanced Developers]]
| These manual instructions are intended for advanced developers. The [Shell Scripts](../01_shell-scripts/index.md) should be the preferred method to build DCD from source. If the script fails or your platform is not supported, continue with the instructions below.

## DCD Dependencies

When performing a manual build, it is necessary to install specific software packages that the DCD software depends on. To learn more about these dependencies, visit the [DCD Software Dependencies](00_dcd-dependencies.md) section.

## Platforms

Shell commands are available to manually download, build, install, test, and uninstall the DCD software and dependencies for these [platforms](03_platforms/index.md).

## Out-of-source Builds

While building dependencies and DCD binaries, out-of-source builds are also supported. Refer to the `cmake` help for more information.

## Other Compilers

To override `clang`'s default compiler toolchain, add these flags to the `cmake` command within the above instructions:

`-DCMAKE_CXX_COMPILER=/path/to/c++ -DCMAKE_C_COMPILER=/path/to/cc`

## Debug Builds

For a debug build, add `-DCMAKE_BUILD_TYPE=Debug`. Other common build types include `Release` and `RelWithDebInfo`.
