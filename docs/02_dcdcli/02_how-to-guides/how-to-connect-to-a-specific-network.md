## Overview
This guide provides instructions on how to connect to specifc DCD blockchain when using `dcdcli`. `dcdcli` can connect to a specific node by using the `--url` optional argument, followed by the http address and port number.

The examples use the `--url`optional argument to send commands to the specified blockchain.   

[[info | Default address:port]]
| If no optional arguments are used (i.e. no `--url`), `dcdcli` attempts to connect to a local `dcdnode` running at localhost or `127.0.0.1` and default port `8888`. Use the `dcdnode` command line arguments or [config.ini](../../01_dcdnode/02_usage/01_dcdnode-configuration/#configini-location) file to specify a different address.

## Before you Begin
Make sure you meet the following requirements: 

* Install the currently supported version of `dcdcli`.
[[info | Note]]
| `dcdcli` is bundled with the DCD software. [Installing DCD](../../00_install/index.md) will install the `dcdcli` and `dcdksd` command line tools.  
* You have access to an DCD blockchain and the http afddress and port number of a `dcdnode` instance. 

## Reference
See the following reference guides for command line usage and related options:

* [dcdcli](../index.md) command

## Example

1. Add the `-url` option to specify the `dcdnode` instance 

```shell
dcdcli -url http://dcdnode-host:8888 COMMAND
```
**Where**
* `-url http://dcdnode-host:8888` = The http address and port number of the `dcdnode` instance to connect to
* COMMAND = The `dcdcli`command.

## Summary
In conclusion, by following these instructions you are able to connect to a specified `dcdnode` instance. 
