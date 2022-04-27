## Goal

Delegate network bandwidth for an account or application.

## Before you begin

Make sure you meet the following requirements:

* Familiarize with the [`dcdcli system delegatebw`](../03_command-reference/system/system-delegatebw.md) command and its parameters.
* Install the currently supported version of `dcdcli`.

[[info | Note]]
| `dcdcli` is bundled with the DCD software. [Installing DCD](../../00_install/index.md) will also install `dcdcli`.

* Ensure the reference system contracts from [`dcd.contracts`](https://github.com/DCD/dcd.contracts) repository is deployed and used to manage system resources.
* Understand what an [account](https://developers.eos.io/welcome/v2.1/glossary/index/#account) is and its role in the blockchain.
* Understand [NET bandwidth](https://developers.eos.io/welcome/v2.1/glossary/index/#net) in an DCD blockchain.
* Understand [CPU bandwidth](https://developers.eos.io/welcome/v2.1/glossary/index/#cpu) in an DCD blockchain.

## Steps

Perform the step below:

Delegate CPU bandwidth from a source account to a receiver account:

```sh
dcdcli system delegatebw <from> <receiver> <stake_net_quantity> <stake_cpu_quantity>
```

Where `from` is the account to delegate bandwidth from, `receiver` is the account to receive the delegated bandwidth, and `stake_net_quantity` and/or `stake_cpu_quantity` is the amount of tokens to stake for network (NET) bandwidth and/or CPU bandwidth, respectively.

Some examples are provided below:

* Delegate 0.01 SYS network bandwidth from `bob` to `alice`:

**Example Output**

```sh
dcdcli system delegatebw bob alice "0.01 SYS" "0 SYS"
```
```json
executed transaction: 5487afafd67bf459a20fcc2dbc5d0c2f0d1f10e33123eaaa07088046fd18e3ae  192 bytes  503 us
#         dcd <= dcd::delegatebw            {"from":"bob","receiver":"alice","stake_net_quantity":"0.0100 SYS","stake_cpu_quantity":"0.0000 SYS"...
#   dcd.token <= dcd.token::transfer        {"from":"bob","to":"dcd.stake","quantity":"0.0010 SYS","memo":"stake bandwidth"}
#  bob <= dcd.token::transfer        {"from":"bob","to":"dcd.stake","quantity":"0.0010 SYS","memo":"stake bandwidth"}
#   dcd.stake <= dcd.token::transfer        {"from":"bob","to":"dcd.stake","quantity":"0.0010 SYS","memo":"stake bandwidth"}
```
