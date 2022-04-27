---
content_title: Download DCD Source
---

To download the DCD source code, clone the `dcd` repo and its submodules. It is adviced to create a home `dcd` folder first and download all the DCD related software there:

```sh
mkdir -p ~/dcd && cd ~/dcd
git clone --recursive https://github.com/DCD/dcd
```

## Update Submodules

If a repository is cloned without the `--recursive` flag, the submodules *must* be updated before starting the build process:

```sh
cd ~/dcd/dcd
git submodule update --init --recursive
```

## Pull Changes

When pulling changes, especially after switching branches, the submodules *must* also be updated. This can be achieved with the `git submodule` command as above, or using `git pull` directly:

```sh
[git checkout <branch>]  (optional)
git pull --recurse-submodules
```

[[info | What's Next?]]
| [Build DCD binaries](02_build-dcd-binaries.md)
