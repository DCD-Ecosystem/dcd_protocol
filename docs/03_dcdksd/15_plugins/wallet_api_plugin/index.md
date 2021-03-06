## Description

The `wallet_api_plugin` exposes functionality from the [`wallet_plugin`](../wallet_plugin/index.md) to the RPC API interface managed by the [`http_plugin`](../../../01_dcdnode/03_plugins/http_plugin/index.md).

[[caution | Caution]]
| This plugin exposes wallets. Therefore, running this plugin on a publicly accessible node is not recommended. As of 1.2.0, the `wallet_api_plugin` is only available through `dcdksd`. It is no longer supported by `dcdnode`.

## Usage

```sh
# config.ini
plugin = dcd::wallet_api_plugin

# command-line
dcdksd ... --plugin dcd::wallet_api_plugin
```

## Options

None

## Dependencies

* [`wallet_plugin`](../wallet_plugin/index.md)
* [`http_plugin`](../../../01_dcdnode/03_plugins/http_plugin/index.md)

### Load Dependency Examples

The following plugins are loaded with default settings if not specified on the command line or `config.ini`:

```sh
# config.ini
plugin = dcd::wallet_plugin
[options]
plugin = dcd::http_plugin
[options]

# command-line
dcdksd ... --plugin dcd::wallet_plugin [options]  \
           --plugin dcd::http_plugin [options]
```
