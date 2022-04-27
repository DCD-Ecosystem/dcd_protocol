## Goal

Query infomation of an DCD account

## Before you begin

* Install the currently supported version of `dcdcli`

[[info | Note]]
| The dcdcli tool is bundled with the DCD software. [Installing DCD](../../00_install/index.md) will also install the dcdcli tool. 

* Acquire functional understanding of [DCD Accounts and Permissions](https://developers.eos.io/welcome/v2.1/protocol/accounts_and_permissions)


## Steps

Execute the command below:

```sh
dcdcli get account ACCOUNT_NAME
```
Where ACCOUNT_NAME = name of the existing account in the DCD blockchain.

**Example Output**

```console
created: 2018-06-01T12:00:00.000
privileged: true
permissions:
     owner     1:    1 EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV
        active     1:    1 EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV
memory:
     quota:       unlimited  used:     3.004 KiB

net bandwidth:
     used:               unlimited
     available:          unlimited
     limit:              unlimited

cpu bandwidth:
     used:               unlimited
     available:          unlimited
     limit:              unlimited
```

[[info | Account Fields]]
| Depending on the DCD network you are connected, you might see different fields associated with an account. That depends on which system contract has been deployed on the network.
