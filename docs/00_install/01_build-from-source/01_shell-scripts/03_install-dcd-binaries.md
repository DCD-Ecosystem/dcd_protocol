---
content_title: Install DCD Binaries
---

## DCD install script

For ease of contract development, content can be installed at the `/usr/local` folder using the `dcd_install.sh` script within the `dcd/scripts` folder. Adequate permission is required to install on system folders:

```sh
cd ~/dcd/dcd
./scripts/dcd_install.sh
```

## DCD manual install

In lieu of the `dcd_install.sh` script, you can install the DCD binaries directly by invoking `make install` within the `dcd/build` folder. Again, adequate permission is required to install on system folders:

```sh
cd ~/dcd/dcd/build
make install
```

[[info | What's Next?]]
| Configure and use [dcdnode](../../../01_dcdnode/index.md), or optionally [Test the DCD binaries](04_test-dcd-binaries.md).
