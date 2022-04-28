---
content_title: dcdksd
---

## Introduction

`dcdksd` is a key manager service daemon for storing private keys and signing digital messages. It provides a secure key storage medium for keys to be encrypted at rest in the associated wallet file. `dcdksd` also defines a secure enclave for signing transaction created by `dcdcli` or a third part library.

## Installation

`dcdksd` is distributed as part of the [DCD software suite](https://github.com/DCD/dcd/blob/master/README.md). To install `dcdksd` just visit the [DCD Software Installation](../00_install/index.md) section.

## Operation

When a wallet is unlocked with the corresponding password, `dcdcli` can request `dcdksd` to sign a transaction with the appropriate private keys. Also, `dcdksd` provides support for hardware-based wallets such as Secure Encalve and YubiHSM.

[[info | Audience]]
| `dcdksd` is intended to be used by DCD developers only.
