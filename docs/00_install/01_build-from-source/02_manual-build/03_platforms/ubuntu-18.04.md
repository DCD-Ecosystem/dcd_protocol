---
content_title: Ubuntu 18.04
---

This section contains shell commands to manually download, build, install, test, and uninstall DCD and dependencies on Ubuntu 18.04.

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
export DCD_LOCATION=~/dcd/eos
export DCD_INSTALL_LOCATION=$DCD_LOCATION/../install
mkdir -p $DCD_INSTALL_LOCATION
# install git
apt-get update && apt-get upgrade -y && DEBIAN_FRONTEND=noninteractive apt-get install -y git
# clone DCD repository
git clone https://github.com/DCD/eos.git $DCD_LOCATION
cd $DCD_LOCATION && git submodule update --init --recursive
```

## Install DCD Dependencies
These commands install the DCD software dependencies. Make sure to [Download the DCD Repository](#download-dcd-repository) first and set the DCD directories.
```sh
# install dependencies
apt-get install -y make bzip2 automake libbz2-dev libssl-dev doxygen graphviz libgmp3-dev \
    autotools-dev python2.7 python2.7-dev python3 python3-dev \
    autoconf libtool curl zlib1g-dev sudo ruby libusb-1.0-0-dev \
    libcurl4-gnutls-dev pkg-config patch llvm-7-dev clang-7 vim-common jq
# build cmake
export PATH=$DCD_INSTALL_LOCATION/bin:$PATH
cd $DCD_INSTALL_LOCATION && curl -LO https://cmake.org/files/v3.13/cmake-3.13.2.tar.gz && \
    tar -xzf cmake-3.13.2.tar.gz && \
    cd cmake-3.13.2 && \
    ./bootstrap --prefix=$DCD_INSTALL_LOCATION && \
    make -j$(nproc) && \
    make install && \
    rm -rf $DCD_INSTALL_LOCATION/cmake-3.13.2.tar.gz $DCD_INSTALL_LOCATION/cmake-3.13.2
# build boost
cd $DCD_INSTALL_LOCATION && curl -LO https://boostorg.jfrog.io/artifactory/main/release/1.71.0/source/boost_1_71_0.tar.bz2 && \
    tar -xjf boost_1_71_0.tar.bz2 && \
    cd boost_1_71_0 && \
    ./bootstrap.sh --prefix=$DCD_INSTALL_LOCATION && \
    ./b2 --with-iostreams --with-date_time --with-filesystem --with-system --with-program_options --with-chrono --with-test -q -j$(nproc) install && \
    rm -rf $DCD_INSTALL_LOCATION/boost_1_71_0.tar.bz2 $DCD_INSTALL_LOCATION/boost_1_71_0
```

## Build DCD
These commands build the DCD software on the specified OS. Make sure to [Install DCD Dependencies](#install-dcd-dependencies) first.

[[caution | `DCD_BUILD_LOCATION` environment variable]]
| Do NOT change this variable. It is set for convenience only. It should always be set to the `build` folder within the cloned repository.

```sh
export DCD_BUILD_LOCATION=$DCD_LOCATION/build
mkdir -p $DCD_BUILD_LOCATION
cd $DCD_BUILD_LOCATION && cmake -DCMAKE_BUILD_TYPE='Release' -DCMAKE_CXX_COMPILER='clang++-7' -DCMAKE_C_COMPILER='clang-7' -DLLVM_DIR='/usr/lib/llvm-7/lib/cmake/llvm' -DCMAKE_INSTALL_PREFIX=$DCD_INSTALL_LOCATION $DCD_LOCATION
cd $DCD_BUILD_LOCATION && make -j$(nproc)
```

## Install DCD
This command installs the DCD software on the specified OS. Make sure to [Build DCD](#build-dcd) first.
```sh
cd $DCD_BUILD_LOCATION && make install
```

## Test DCD
These commands validate the DCD software installation on the specified OS. Make sure to [Install DCD](#install-dcd) first. (**Note**: This task is optional but recommended.)
```sh
cd $DCD_BUILD_LOCATION && make test
```

## Uninstall DCD
These commands uninstall the DCD software from the specified OS.
```sh
xargs rm < $DCD_BUILD_LOCATION/install_manifest.txt
rm -rf $DCD_BUILD_LOCATION
```
