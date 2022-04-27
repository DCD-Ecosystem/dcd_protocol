## Description
Retrieves the code and ABI for an account

## Positional Parameters
- `name` _TEXT_ - The name of the account whose code should be retrieved
## Options
- `-c,--code` _TEXT_ - The name of the file to save the contract _.wast_ to
- `-a,--abi` _TEXT_ - The name of the file to save the contract _.abi_ to
- `--wasm` Save contract as wasm
## Examples
Simply output the hash of dcd.token contract 

```sh
dcdcli get code dcd.token
```
```console
code hash: f675e7aeffbf562c033acfaf33eadff255dacb90d002db51c7ad7cbf057eb791
```

Retrieve and save abi for dcd.token contract

```sh
dcdcli get code dcd.token -a dcd.token.abi
```
```console
code hash: f675e7aeffbf562c033acfaf33eadff255dacb90d002db51c7ad7cbf057eb791
saving abi to dcd.token.abi
```

Retrieve and save wast code for dcd.token contract

```sh
dcdcli get code dcd.token -c dcd.token.wast
```
```console
code hash: f675e7aeffbf562c033acfaf33eadff255dacb90d002db51c7ad7cbf057eb791
saving wast to dcd.token.wast
```
