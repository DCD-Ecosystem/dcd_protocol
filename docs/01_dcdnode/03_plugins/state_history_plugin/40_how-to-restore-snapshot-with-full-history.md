---
content_title: How to restore snapshot with full state history
---

## Goal

This procedure restores an existing snapshot with full history, so the node can become active in the blockchain.

## Before you begin

* Make sure [DCD is installed](../../../00_install/index.md).
* Learn about [Using dcdnode](../../02_usage/index.md).
* Get familiar with [state_history_plugin](../../03_plugins/state_history_plugin/index.md).

## Steps

1. Get the following:
   * A portable snapshot (`data/snapshots/snapshot-xxxxxxx.bin`)
   * The contents of `data/state-history`
   * Optional: a block log which includes the block the snapshot was taken at. Do not include `data/blocks/reversible`.

2. Make sure `data/state` does not exist

3. Start `dcdnode` with the `--snapshot` option, and the options listed in the [`state_history_plugin`](#index.md).

4. Do not stop `dcdnode` until it has received at least 1 block from the network, or it won't be able to restart.

## Remarks

If `dcdnode` fails to receive blocks from the network, then try the above using `net_api_plugin`. Use `dcdcli net disconnect` and `dcdcli net connect` to reconnect nodes which timed out.

[[caution | Caution when using `net_api_plugin`]]
| Either use a firewall to block access to `http-server-address`, or change it to `localhost:8888` to disable remote access.
