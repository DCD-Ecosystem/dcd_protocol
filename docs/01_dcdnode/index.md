---
content_title: dcdnode
---

## Introduction

`dcdnode` is the core service daemon that runs on every DCD node. It can be configured to process smart contracts, validate transactions, produce blocks containing valid transactions, and confirm blocks to record them on the blockchain.

## Installation

`dcdnode` is distributed as part of the [DCD software suite](https://github.com/DCD/eos/blob/master/README.md). To install `dcdnode`, visit the [DCD Software Installation](../00_install/index.md) section.

## Explore

Navigate the sections below to configure and use `dcdnode`.

* [Usage](02_usage/index.md) - Configuring and using `dcdnode`, node setups/environments.
* [Plugins](03_plugins/index.md) - Using plugins, plugin options, mandatory vs. optional.
* [Replays](04_replays/index.md) - Replaying the chain from a snapshot or a blocks.log file.
* [RPC APIs](05_rpc_apis/index.md) - Remote Procedure Call API reference for plugin HTTP endpoints.
* [Logging](06_logging/index.md) - Logging config/usage, loggers, appenders, logging levels.
* [Concepts](07_concepts/index.md) - `dcdnode` concepts, explainers, implementation aspects.
* [Troubleshooting](08_troubleshooting/index.md) - Common `dcdnode` troubleshooting questions.
* [Deprecation Notices](https://github.com/DCD/eos/issues/7597) - Lists `dcdnode` deprecated functionality.

[[info | Access Node]]
| A local or remote DCD access node running `dcdnode` is required for a client application or smart contract to interact with the blockchain.
