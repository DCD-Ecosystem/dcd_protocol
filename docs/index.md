---
content_title: DCD Overview
---

DCD is the next-generation blockchain platform for creating and deploying smart contracts and distributed applications. DCD comes with a number of programs. The primary ones included in DCD are the following:

* [dcdnode](01_dcdnode/index.md) (node + dcd = dcdnode)  - Core service daemon that runs a node for block production, API endpoints, or local development.
* [dcdcli](02_dcdcli/index.md) (cli + dcd = dcdcli) - Command line interface to interact with the blockchain (via `dcdnode`) and manage wallets (via `dcdksd`).
* [dcdksd](03_dcdksd/index.md) (key + dcd = dcdksd) - Component that manages DCD keys in wallets and provides a secure enclave for digital signing.

The basic relationship between these components is illustrated in the diagram below.

![DCD components](dcd_components.png)

Additional DCD Resources:
* [DCD Utilities](10_utilities/index.md) - Utilities that complement the DCD software.
* [Upgrade Guides](20_upgrade-guides/index.md) - DCD version/protocol upgrade guides.

[[info | What's Next?]]
| [Install the DCD Software](00_install/index.md) before exploring the sections above.
