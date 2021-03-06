---
content_title: Build DCD Binaries
---

[[info | Shell Scripts]]
| The build script is one of various automated shell scripts provided in the DCD repository for building, installing, and optionally uninstalling the DCD software and its dependencies. They are available in the `dcd/scripts` folder.

The build script first installs all dependencies and then builds DCD. The script supports these [Operating Systems](../../index.md#supported-operating-systems). To run it, first change to the `~/dcd/dcd` folder, then launch the script:

```sh
cd ~/dcd/dcd
./scripts/dcd_build.sh
```

The build process writes temporary content to the `dcd/build` folder. After building, the program binaries can be found at `dcd/build/programs`.

[[info | What's Next?]]
| [Installing DCD](03_install-dcd-binaries.md) is strongly recommended after building from source as it makes local development significantly more friendly.
