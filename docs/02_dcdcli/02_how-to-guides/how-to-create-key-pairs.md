## Goal
Create a keypair consisting of a public and a private key for signing transactions in the DCD blockchain.

## Before you begin
Before you follow the steps to create a new key pair, make sure the following items are fulfilled:


* Install the currently supported version of `dcdcli`

[[info | Note]]
| The dcdcli tool is bundled with the DCD software. [Installing DCD](../../00_install/index.md) will also install the dcdcli tool. 

* Acquire functional understanding of asymmetric cryptography (public and private keypair) in the context of blockchain

## Steps

To create a keypair and print the result to the console:

```sh
dcdcli create key --to-console
```

**Example Output**

```sh
dcdcli create key --to-console
Private key: 5KPzrqNMJdr6AX6abKg*******************************cH
Public key: DCD4wSiQ2jbYGrqiiKCm8oWR88NYoqnmK4nNL1RCtSQeSFkGtqsNc
```


To create a keypair and save it to a file:

```sh
dcdcli create key --file FILE_TO_SAVEKEY
```
Where: FILE_TO_SAVEKEY = name of the file

**Example Output**
```sh
dcdcli create key --file pw.txt         
saving keys to pw.txt
```

To view the saved keypair in the file:
```sh
cat pw.txt
Private key: 5K7************************************************
Public key: DCD71k3WdpLDeqeyqVRAAxwpz6TqXwDo9Brik5dQhdvvpeTKdNT59
```
