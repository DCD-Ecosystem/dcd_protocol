## Goal

Unstake resource from your account

Beware that only the account which originally delegated resource can undelegate

## Before you begin

* Install the currently supported version of `dcdcli`

* Ensure the reference system contracts from `dcd.contracts` repository is deployed and used to manage system resources

* Understand the following:
  * What is an account
  * What is network bandwidth
  * What is CPU bandwidth

## Steps

Unstake 0.01 DCD CPU bandwidth form account `alice`:

```sh
dcdcli system undelegatebw alice alice "0.01 DCD" "0 DCD"
```

You should see something below:

```console
executed transaction: e7e7edb6c5556de933f9d663fea8b4a9cd56ece6ff2cebf056ddd0835efa6606  184 bytes  452 us
#         dcd <= dcd::undelegatebw          {"from":"alice","receiver":"alice","unstake_net_quantity":"0.0000 DCD","unstake_cpu_qu...
warning: transaction executed locally, but may not be confirmed by the network yet         ]
```
