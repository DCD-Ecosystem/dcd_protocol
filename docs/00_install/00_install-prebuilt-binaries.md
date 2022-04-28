---
content_title: Install Prebuilt Binaries
---

[[info | Previous Builds]]
| If you have previously installed DCD from source using shell scripts, you must first run the [Uninstall Script](01_build-from-source/01_shell-scripts/05_uninstall-dcd.md) before installing any prebuilt binaries on the same OS.

## Prebuilt Binaries

Prebuilt DCD software packages are available for the operating systems below. Find and follow the instructions for your OS:

### Mac OS X:

#### Mac OS X Brew Install
```sh
brew tap dcd/dcd
brew install dcd
```
#### Mac OS X Brew Uninstall
```sh
brew remove dcd
```

### Ubuntu Linux:
#### Ubuntu 20.04 Package Install
```sh
wget https://github.com/dcd/dcd/releases/download/v2.1.0/dcd_2.1.0-1-ubuntu-20.04_amd64.deb
sudo apt install ./dcd_2.1.0-1-ubuntu-20.04_amd64.deb
```
#### Ubuntu 18.04 Package Install
```sh
wget https://github.com/dcd/dcd/releases/download/v2.1.0/dcd_2.1.0-1-ubuntu-18.04_amd64.deb
sudo apt install ./dcd_2.1.0-1-ubuntu-18.04_amd64.deb
```
#### Ubuntu 16.04 Package Install
```sh
wget https://github.com/dcd/dcd/releases/download/v2.1.0/dcd_2.1.0-1-ubuntu-16.04_amd64.deb
sudo apt install ./dcd_2.1.0-1-ubuntu-16.04_amd64.deb
```
#### Ubuntu Package Uninstall
```sh
sudo apt remove dcd
```

### RPM-based (CentOS, Amazon Linux, etc.):

#### RPM Package Install CentOS 7
```sh
wget https://github.com/dcd/dcd/releases/download/v2.1.0/dcd-2.1.0-1.el7.x86_64.rpm
sudo yum install ./dcd-2.1.0-1.el7.x86_64.rpm
```
#### RPM Package Install CentOS 8
```sh
wget https://github.com/dcd/dcd/releases/download/v2.1.0/dcd-2.1.0-1.el8.x86_64.rpm
sudo yum install ./dcd-2.1.0-1.el8.x86_64.rpm
```
#### RPM Package Uninstall
```sh
sudo yum remove dcd
```

## Location of DCD binaries

After installing the prebuilt packages, the actual DCD binaries will be located under:
* `/usr/opt/dcd/<version-string>/bin` (Linux-based); or
* `/usr/local/Cellar/dcd/<version-string>/bin` (MacOS)

where `version-string` is the DCD version that was installed.

Also, soft links for each DCD program (`dcdnode`, `dcdcli`, `dcdksd`, etc.) will be created under `usr/bin` or `usr/local/bin` to allow them to be executed from any directory.

## Previous Versions

To install previous versions of the DCD prebuilt binaries:

1. Browse to https://github.com/DCD/dcd/tags and select the actual version of the DCD software you need to install.

2. Scroll down past the `Release Notes` and download the package or archive that you need for your OS.

3. Follow the instructions on the first paragraph above to install the selected prebuilt binaries on the given OS.
