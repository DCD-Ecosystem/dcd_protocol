## Goal

List all key pairs

## Before you begin

* Install the currently supported version of `dcdcli`

* Understand the following:
  * What is a public and private key pair

## Steps

Unlock your wallet

```sh
dcdcli wallet unlock
```

List all public keys:

```sh
dcdcli wallet keys
```

List all private keys:

```sh
dcdcli wallet private_keys

```

[[warning]]
| Be careful never real your private keys in a production enviroment
