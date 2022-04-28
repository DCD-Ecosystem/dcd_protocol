## Description
The `net_api_plugin` exposes functionality from the `net_plugin` to the RPC API interface managed by the `http_plugin`. Node operators can use the `net_api_plugin` to manage the p2p connections of an active node.

The `net_api_plugin` provides four RPC API endpoints:

* connect
* disconnect
* connections
* status

See [Net API Reference Documentation](https://developers.dcd.io/manuals/dcd/latest/dcdnode/plugins/net_api_plugin/api-reference/index).

[[caution | Caution]]
| This plugin exposes endpoints that allow management of p2p connections. Running this plugin on a publicly accessible node is not recommended as it can be exploited.

## Usage

```console
# config.ini
plugin = dcd::net_api_plugin
```
```sh
# command-line
dcdnode ... --plugin dcd::net_api_plugin
```

## Options

None

## Dependencies

* [`net_plugin`](../net_plugin/index.md)
* [`http_plugin`](../http_plugin/index.md)

### Load Dependency Examples

```console
# config.ini
plugin = dcd::net_plugin
[options]
plugin = dcd::http_plugin
[options]
```
```sh
# command-line
dcdnode ... --plugin dcd::net_plugin [options]  \
           --plugin dcd::http_plugin [options]
```
