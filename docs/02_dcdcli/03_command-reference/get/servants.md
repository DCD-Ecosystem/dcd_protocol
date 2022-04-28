## Description
Retrieve accounts which are servants of a given account 

## Info

**Command**

```sh
dcdcli get servants
```
**Output**

```console
Usage: dcdcli get servants account

Positionals:
  account TEXT                The name of the controlling account
```

## Command

```sh
dcdcli get servants inita
```

## Output

```json
{
  "controlled_accounts": [
    "tester"
  ]
}
```
