## Description
Retrieve the stats of for a given currency

## Positional Parameters
`contract` _TEXT_  - The contract that operates the currency

`symbol` _TEXT_ - The symbol for the currency if the contract operates multiple currencies
## Options
There are no options for this subcommand
## Example
Get stats of the DCD token from the dcd.token contract. 

```sh
dcdcli get currency stats dcd.token SYS
```
```json
{
  "DCD": {
    "supply": "1000000000.0000 DCD",
    "max_supply": "10000000000.0000 DCD",
    "issuer": "dcd"
  }
}
```
