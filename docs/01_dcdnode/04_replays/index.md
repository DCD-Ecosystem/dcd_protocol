---
content_title: dcdnode Replays
---

dcdnode provides various options for replaying blockchain blocks. This can be useful if, for example, a node has downloaded a `blocks.log` file from the internet (as a faster alternative to synchronizing from the p2p network) and the node wants to use it to quickly catch up with the network, or if you want to know the chain state at specified points in a blockchain's life.

Replaying data can be done in two ways:

- From a **`blocks.log` file**:  
The `blocks.log` file contains all irreversible transactions on the blockchain. All instances of `dcdnode` write irreversible blocks to the `blocks.log` file, which is located at the `data/blocks` directory relative to the `dcdnode` directory. Using a `blocks.log` file to replay will allow you to start a `dcdnode` instance, which recreates the entire history of the blockchain locally, without adding unnecessary load to the network.

- From a **snapshot file**:  
Snapshot files can be created from a running `dcdnode` instance. The snapshot contains the chain state for the block referenced when created. It is recommended to use snapshot files created from blocks that are irreversible. Using a snapshot file to replay allows you to quickly start a `dcdnode` instance which has a full and correct chain state at a specified block number, but not a full history of transactions up to that block number. From that point on the `dcdnode` instance will operate in the configured manner.

## Replay How-Tos

* [How To Generate a Blocks Log](how-to-generate-a-blocks.log.md)
* [How To Generate a Snapshot](how-to-generate-a-snapshot.md)
* [How To Replay from a Blocks Log](how-to-replay-from-a-blocks.log.md)
* [How to Replay from a Snapshot](how-to-replay-from-a-snapshot.md)

## Replay Snapshot-specific Options

Typing `dcdnode --help` on the command line will show you all the options available for running `dcdnode`. The snapshot and replay specific options are:

 - **--force-all-checks**  
The node operator may not trust the source of the `blocks.log` file and may want to run `dcdnode` with `--replay-blockchain --force-all-checks` the first time to make sure the blocks are good. The `--force-all-checks` flag can be passed into `dcdnode` to tell it to not skip any checks during replay.

 - **--disable-replay-opts**  
By default, during replay, `dcdnode` does not create a stack of chain state deltas (this stack is used to enable rollback of state for reversible blocks.) This is a replay performance optimization. Using this option turns off this replay optimization and creates a stack of chain state deltas. If you are using the state history plugin you must use this option.

 - **--replay-blockchain**  
This option tells `dcdnode` to replay from the `blocks.log` file located in the data/blocks directory. `dcdnode` will clear the chain state and replay all blocks.

 - **--hard-replay-blockchain**  
This option tells `dcdnode` to replay from the `blocks.log` file located in the data/blocks directory. `dcdnode` makes a backup of the existing `blocks.log` file and will then clear the chain state and replay all blocks. This option assumes that the backup `blocks.log` file may contain corrupted blocks, so `dcdnode` replays as many blocks as possible from the backup block log. When `dcdnode` finds the first corrupted block while replying from `dcdnode.log` it will synchronize the rest of the blockchain from the p2p network.

 - **--delete-all-blocks**  
This tells `dcdnode` to clear the local chain state and local the `blocks.log` file, If you intend to then synchronize from the p2p network you would need to provide a correct `genesis.json` file. This option is not recommended.

 - **--truncate-at-block**  
Default argument (=0), only used if the given value is non-zero.
Using this option when replaying the blockchain will force the replay to stop at the specified block number. This option will only work if replaying with the `--hard-replay-blockchain` option, or, when not replaying, the `--fix-reversible-blocks` option. The local `dcdnode` process will contain the chain state for that block. This option may be useful for checking blockchain state at specific points in time. It is intended for testing/validation and is not intended to be used when creating a local `dcdnode` instance which is synchronized with the network.  
 
 - **--snapshot**  
Use this option to specify which snapshot file to use to recreate the chain state from a snapshot file. This option will not replay the `blocks.log` file. The `dcdnode` instance will not know the full transaction history of the blockchain. 

 - **--snapshots-dir**  
You can use this to specify the location of the snapshot file directory  (absolute path or relative to application data dir.)

 - **--blocks-dir**  
You can use this to specify the location of the `blocks.log` file directory  (absolute path or relative to application data dir)
