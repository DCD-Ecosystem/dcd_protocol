## Description

Retrieve the balance of an account for a given currency

## Positional Parameters
`contract` _TEXT_ - The contract that operates the currency

`account` _TEXT_ - The account to query balances for

`symbol` _TEXT_ - The symbol for the currency if the contract operates multiple currencies

## Options
There are no options for this subcommand

## Example
Get balance of dcd from dcd.token contract for DCD symbol. 

```sh
dcdcli get currency balance dcd.token dcd SYS
```
```console
999999920.0000 SYS
```
