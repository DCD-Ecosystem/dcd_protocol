## Goal
Create a new DCD blockchain account 

## Before you begin

* Install the currently supported version of `dcdcli`

[[info | Note]]
| The dcdcli tool is bundled with the DCD software. [Installing DCD](../../00_install/index.md) will also install the dcdcli tool. 

* Acquire functional understanding of the following:
  * [DCD Accounts and Permissions](https://developers.eos.io/welcome/v2.1/protocol/accounts_and_permissions)
  * Asymmetric cryptography (public and private keypair) 

* Created an Owner and an Active key pair
* [Import a private key](../02_how-to-guides/how-to-import-a-key.md) to the wallet which can authorize on behalf of a creator account

## Steps

**Command**

```sh
dcdcli create account [creator account name] [new account name][OwnerKey] [ActiveKey]
```
Where:

[creator account name] = name of the existing account that authorizes the creation of a new account

[new account name] = The name of the new account account adhering to DCD account naming conventions

[OwnerKey] = The owner permissions linked to the ownership of the account

[ActiveKey] = The active permissions linked with the creator account

[[info | Recommend]]
| `ActiveKey` is optional but recommended.

[[info | Note]]
| To create a new account in the DCD blockchain, an existing account, also referred to as a creator account, is required to authorize the creation of a new account. For a newly created DCD blockchain, the default system account used to create a new account is dcd.

**Example Output**
```sh
dcdcli create account dcd bob EOS87TQktA5RVse2EguhztfQVEh6XXxBmgkU8b4Y5YnGvtYAoLGNN
executed transaction: 4d65a274de9f809f9926b74c3c54aadc0947020bcfb6dd96043d1bcd9c46604c  200 bytes  166 us
#         dcd <= dcd::newaccount            {"creator":"dcd","name":"bob","owner":{"threshold":1,"keys":[{"key":"EOS87TQktA5RVse2EguhztfQVEh6X...
warning: transaction executed locally, but may not be confirmed by the network yet         ]
```
