---
content_title: MacOS 10.14
---

This section contains shell commands to manually download, build, install, test, and uninstall DCD and dependencies on MacOS 10.14.

[[info | Building DCD is for Advanced Developers]]
| If you are new to DCD, it is recommended that you install the [DCD Prebuilt Binaries](../../../00_install-prebuilt-binaries.md) instead of building from source.

Select a task below, then copy/paste the shell commands to a Unix terminal to execute:

* [Download DCD Repository](#download-dcd-repository)
* [Install DCD Dependencies](#install-dcd-dependencies)
* [Build DCD](#build-dcd)
* [Install DCD](#install-dcd)
* [Test DCD](#test-dcd)
* [Uninstall DCD](#uninstall-dcd)

[[info | Building DCD on another OS?]]
| Visit the [Build DCD from Source](../../index.md) section.

## Download DCD Repository
These commands set the DCD directories, install git, and clone the DCD repository.
```sh
# set DCD directories
export DCD_LOCATION=~/dcd/dcd
export DCD_INSTALL_LOCATION=$DCD_LOCATION/../install
mkdir -p $DCD_INSTALL_LOCATION
# install git
brew update && brew install git
# clone DCD repository
git clone https://github.com/DCD/dcd.git $DCD_LOCATION
cd $DCD_LOCATION && git submodule update --init --recursive
```

## Install DCD Dependencies
These commands install the DCD software dependencies. Make sure to [Download the DCD Repository](#download-dcd-repository) first and set the DCD directories.
```sh
# install dependencies
brew install cmake python libtool libusb graphviz automake wget gmp pkgconfig doxygen openssl@1.1 jq boost || :
export PATH=$DCD_INSTALL_LOCATION/bin:$PATH
```

## Build DCD
These commands build the DCD software on the specified OS. Make sure to [Install DCD Dependencies](#install-dcd-dependencies) first.

[[caution | `DCD_BUILD_LOCATION` environment variable]]
| Do NOT change this variable. It is set for convenience only. It should always be set to the `build` folder within the cloned repository.

```sh
export DCD_BUILD_LOCATION=$DCD_LOCATION/build
mkdir -p $DCD_BUILD_LOCATION
cd $DCD_BUILD_LOCATION && cmake -DCMAKE_BUILD_TYPE='Release' -DCMAKE_INSTALL_PREFIX=$DCD_INSTALL_LOCATION $DCD_LOCATION
cd $DCD_BUILD_LOCATION && make -j$(getconf _NPROCESSORS_ONLN)
```

## Install DCD
This command installs the DCD software on the specified OS. Make sure to [Build DCD](#build-dcd) first.
```sh
cd $DCD_BUILD_LOCATION && make install
```

## Test DCD
These commands validate the DCD software installation on the specified OS. This task is optional but recommended. Make sure to [Install DCD](#install-dcd) first.
```sh
cd $DCD_BUILD_LOCATION && make test
```

## Uninstall DCD
These commands uninstall the DCD software from the specified OS.
```sh
xargs rm < $DCD_BUILD_LOCATION/install_manifest.txt
rm -rf $DCD_BUILD_LOCATION
```
