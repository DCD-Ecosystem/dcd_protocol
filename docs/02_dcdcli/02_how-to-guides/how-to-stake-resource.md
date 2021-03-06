## Overview

This how-to guide provides instructions on how to stake resources, NET and/or CPU, for your account using the `dcdcli` CLI tool.

## Before you begin

* Install the currently supported version of `dcdcli`.

* Ensure the [reference system contracts](https://developers.dcd.io/manuals/dcd.contracts/v1.9/build-and-deploy) are deployed and used to manage system resources.

* Understand the following:
  * What an [account](https://developers.dcd.io/welcome/v2.1/glossary/index/#account) is.
  * What [NET bandwidth](https://developers.dcd.io/manuals/dcd.contracts/v1.9/key-concepts/net) is.
  * What [CPU bandwidth](https://developers.dcd.io/manuals/dcd.contracts/v1.9/key-concepts/cpu) is.
  * The [`delegatebw` dcdcli sub-command](https://developers.dcd.io/manuals/dcd/v2.1/dcdcli/command-reference/system/system-delegatebw).

## Command Reference

See the following reference guides for command line usage and related options for the `dcdcli` command:

* The [`delegatebw` dcdcli sub-command](https://developers.dcd.io/manuals/dcd/v2.1/dcdcli/command-reference/system/system-delegatebw).

## Procedure

The following steps show:

1. [How to stake NET bandwidth.](#1-stake-net-bandwidth)
2. [How to stake CPU bandwidth.](#2-stake-cpu-bandwidth)
3. [How to stake NET and CPU bandwidth.](#3-stake-net-and-cpu-bandwidth)

### 1. Stake NET bandwidth

Run the following command to stake `0.01 DCD` of NET bandwidth for `alice` account from `bob` account:

```sh
dcdcli system delegatebw alice bob "0.01 DCD" "0 DCD"
```

Where:

* `alice` = the account for which the NET bandwidth is staked.
* `bob` = the account that pays the `0.01 DCD` for the NET bandwidth staked.
* `0.01 DCD` = the amount of `DCD` tokens allocated to stake NET bandwidth.
* `0 DCD` = the amount of `DCD` tokens allocated to stake CPU bandwidth.

Example output:

```console
executed transaction: 5487afafd67bf459a20fcc2dbc5d0c2f0d1f10e33123eaaa07088046fd18e3ae  192 bytes  503 us
#         dcd <= dcd::delegatebw            {"from":"bob","receiver":"alice","stake_net_quantity":"0.01 DCD","stake_cpu_quanti...
#   dcd.token <= dcd.token::transfer        {"from":"bob","to":"dcd.stake","quantity":"0.01 DCD","memo":"stake bandwidth"}
#  alice <= dcd.token::transfer        {"from":"bob","to":"dcd.stake","quantity":"0.01 DCD","memo":"stake bandwidth"}
#   dcd.stake <= dcd.token::transfer        {"from":"bob","to":"dcd.stake","quantity":"0.01 DCD","memo":"stake bandwidth"}
```

### 2. Stake CPU bandwidth

Run the following command to stake `0.01 DCD` of CPU bandwidth for `alice` account from `bob` account:

```sh
dcdcli system delegatebw alice bob "0 DCD" "0.01 DCD"
```

Where:

* `alice` = the account for which the CPU bandwidth is staked.
* `bob` = the account that pays the `0.01 DCD` for the CPU bandwidth staked.
* `0 DCD` = the amount of `DCD` tokens allocated to stake NET bandwidth.
* `0.01 DCD` = the amount `DCD` tokens allocated to stake CPU bandwidth.

Example output:

```console
executed transaction: 5487afafd67bf459a20fcc2dbc5d0c2f0d1f10e33123eaaa07088046fd18e3ae  192 bytes  503 us
#         dcd <= dcd::delegatebw            {"from":"bob","receiver":"alice","stake_net_quantity":"0.0000 DCD","stake_cpu_quanti...
#   dcd.token <= dcd.token::transfer        {"from":"bob","to":"dcd.stake","quantity":"0.01 DCD","memo":"stake bandwidth"}
#  alice <= dcd.token::transfer        {"from":"bob","to":"dcd.stake","quantity":"0.01 DCD","memo":"stake bandwidth"}
#   dcd.stake <= dcd.token::transfer        {"from":"bob","to":"dcd.stake","quantity":"0.01 DCD","memo":"stake bandwidth"}
```

### 3. Stake NET and CPU bandwidth

Run the following command to stake `0.01 DCD` of NET and `0.02 DCD` of CPU bandwidth for `alice` account from `bob` account:

```sh
dcdcli system delegatebw alice bob "0.01 DCD" "0.02 DCD"
```

Where:

* `alice` = the account for which the NET and CPU bandwidth is staked.
* `bob` = the account that pays `0.01 DCD` for the NET and `0.02 DCD` for the CPU bandwidth staked.
* `0.01 DCD` = the amount of `DCD` tokens allocated to stake NET bandwidth.
* `0.02 DCD` = the amount of `DCD` tokens allocated to stake CPU bandwidth.

Example output:

```console
executed transaction: 5487afafd67bf459a20fcc2dbc5d0c2f0d1f10e33123eaaa07088046fd18e3ae  192 bytes  503 us
#         dcd <= dcd::delegatebw            {"from":"bob","receiver":"alice","stake_net_quantity":"0.01 DCD","stake_cpu_quanti...
#   dcd.token <= dcd.token::transfer        {"from":"bob","to":"dcd.stake","quantity":"0.01 DCD","memo":"stake bandwidth"}
#  alice <= dcd.token::transfer        {"from":"bob","to":"dcd.stake","quantity":"0.01 DCD","memo":"stake bandwidth"}
#   dcd.stake <= dcd.token::transfer        {"from":"bob","to":"dcd.stake","quantity":"0.01 DCD","memo":"stake bandwidth"}
```

[[info|An account can stake to itself]]
| An account can stake resources to itself, that is, `bob` account can be substituted in the above examples with `alice`, provided `alice` account holds sufficient `DCD` tokens. That means `alice` account stakes resources to itself.

## Summary

In conclusion, the above instructions show how to stake CPU and/or NET bandwidth from one account to another or to itself.
