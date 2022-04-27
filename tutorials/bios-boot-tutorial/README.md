# Bios Boot Tutorial

The `bios-boot-tutorial.py` script simulates the DCD bios boot sequence.

## Prerequisites

1. Python 3.x
2. CMake
3. git

## Steps

1. Install dcd binaries by following the steps outlined in below tutorial
[Install dcd binaries](https://github.com/DCD/eos#mac-os-x-brew-install)

2. Install dcd.cdt binaries by following the steps outlined in below tutorial
[Install dcd.cdt binaries](https://github.com/DCD/dcd.cdt#binary-releases)

3. Compile dcd.contracts sources repository by following the [compile dcd.contracts guidelines](https://github.com/DCD/dcd.contracts/blob/master/docs/02_compile-and-deploy.md) first part, the deploying steps from those guidelines should not be executed.

4. Make note of the full path of the directory where the contracts were compiled, if you followed the [compile dcd.contracts guidelines](https://github.com/DCD/dcd.contracts/blob/master/docs/02_compile-and-deploy.md) it should be under the `build` folder, in `build/contracts/`, we'll reference it from now on as `DCD_CONTRACTS_DIRECTORY`

5. Launch the `bios-boot-tutorial.py` script
Minimal command line to launch the script below, make sure you replace `DCD_CONTRACTS_DIRECTORY` with actual directory

```bash
$ cd ~
$ git clone https://github.com/DCD/eos.git
$ cd ./eos/tutorials/bios-boot-tutorial/
$ python3 bios-boot-tutorial.py --dcdcli=dcdcli --dcdnode=dcdnode --dcdksd=dcdksd --contracts-dir="DCD_CONTRACTS_DIRECTORY" -w -a

```

See [DCD Documentation Wiki: Tutorial - Bios Boot](https://github.com/DCD/eos/wiki/Tutorial-Bios-Boot-Sequence) for additional information.
