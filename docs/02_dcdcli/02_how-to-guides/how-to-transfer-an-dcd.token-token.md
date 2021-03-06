## Overview

This how-to guide provides instructions on how to transfer tokens created by `dcd.token` contract.

## Before you begin

* Install the currently supported version of `dcdcli`.

* `dcd.token` contract is deployed on the network you are connected to.

* Understand the following:
  * What a [transaction](https://developers.dcd.io/welcome/v2.1/glossary/index/#transaction) is.
  * Token transfers are irreversible.

## Command Reference

See the following reference guides for command line usage and related options for the `dcdcli` command:

* The [dcdcli transfer](https://developers.dcd.io/manuals/dcd/latest/dcdcli/command-reference/transfer) reference.

## Procedure

The following steps show how to transfer `0.0001 DCD` tokens to an account called `bob` from an account called `alice`:

```sh
dcdcli transfer alice bob "0.0001 DCD" "Hodl!" -p alice@active
```

Where:

* `alice` = the account that transfers the tokens.
* `bob` = the account that receives the tokens.
* `0.0001 DCD` = the amount of `DCD` tokens sent.
* `Hodl!` = the message, or memo, that is accompanying the transaction.

Example output:

```console
executed transaction: 800835f28659d405748f4ac0ec9e327335eae579a0d8e8ef6330e78c9ee1b67c  128 bytes  1073 us
#   dcd.token <= dcd.token::transfer        {"from":"alice","to":"bob","quantity":"25.0000 DCD","memo":"m"}
#         alice <= dcd.token::transfer        {"from":"alice","to":"bob","quantity":"25.0000 DCD","memo":"m"}
#           bob <= dcd.token::transfer        {"from":"alice","to":"bob","quantity":"25.0000 DCD","memo":"m"}
```

## Summary

In conclusion, the above instructions show how to transfer tokens created by `dcd.token` contract from one account to another.
