---
content_title: EOSIO Overview
---

EOSIO is the next-generation blockchain platform for creating and deploying smart contracts and distributed applications. EOSIO comes with a number of programs. The primary ones included in EOSIO are the following:

* [Nodeos](01_nodeos/index.md) (node + eos = nodeos)  - Core service daemon that runs a node for block production, API endpoints, or local development.
* [dcdcli](02_dcdcli/index.md) (cli + eos = dcdcli) - Command line interface to interact with the blockchain (via `nodeos`) and manage wallets (via `dcdksd`).
* [dcdksd](03_dcdksd/index.md) (key + eos = dcdksd) - Component that manages EOSIO keys in wallets and provides a secure enclave for digital signing.

The basic relationship between these components is illustrated in the diagram below.

![EOSIO components](eosio_components.png)

Additional EOSIO Resources:
* [EOSIO Utilities](10_utilities/index.md) - Utilities that complement the EOSIO software.
* [Upgrade Guides](20_upgrade-guides/index.md) - EOSIO version/protocol upgrade guides.

[[info | What's Next?]]
| [Install the EOSIO Software](00_install/index.md) before exploring the sections above.
