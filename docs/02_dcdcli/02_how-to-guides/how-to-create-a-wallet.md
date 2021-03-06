## Goal

Create a wallet using `dcdksd`.

## Before you begin

Make sure you meet the following requirements:

* Familiarize with the [`dcdcli wallet create`](../03_command-reference/wallet/create.md) command and its parameters.
* Familiarize with the other [`dcdcli wallet`](../03_command-reference/wallet/index.md) commands.
* Install the currently supported version of `dcdcli`.

[[info | Note]]
| `dcdcli` is bundled with the DCD software. [Installing DCD](../../00_install/index.md) will also install `dcdcli`.

* Understand what an [account](https://developers.dcd.io/welcome/v2.1/glossary/index/#account) is and its role in the blockchain.
* Understand [Accounts and Permissions](https://developers.dcd.io/welcome/v2.1/protocol-guides/accounts_and_permissions) in the protocol documents.
* Understand what a [public](https://developers.dcd.io/welcome/v2.1/glossary/index/#public-key) and [private](https://developers.dcd.io/welcome/v2.1/glossary/index/#private-key) key pair is.

## Steps

Perform the step below:

Create a default or named wallet and save the wallet password to a file:

```sh
dcdcli wallet create [-n named_wallet] -f <file_to_save_pwd>
```

Where `file_to_save_pwd` is the name of the file to write the wallet password to and `named_wallet` is an optional parameter to assign a name to the wallet.

Some examples are provided below:

* Create a default wallet and save the password to the file `default_wallet.pwd`:

**Example Output**

```sh
dcdcli wallet create -f default_wallet.pwd
```
```console
Creating wallet: default
Save password to use in the future to unlock this wallet.
Without password imported keys will not be retrievable.
saving password to default_wallet.pwd
```

* Create a named wallet `my_wallet` and save the password to the file `my_wallet.pwd`:

**Example Output**

```sh
dcdcli wallet create -n my_wallet -f my_wallet.pwd
```
```console
Creating wallet: my_wallet
Save password to use in the future to unlock this wallet.
Without password imported keys will not be retrievable.
saving password to my_wallet.pwd
```
