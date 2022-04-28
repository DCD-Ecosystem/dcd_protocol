#!/bin/bash
DATADIR="/node/blockchain"
LOGDIR="/node/log"

if [ ! -d $DATADIR ]; then
  mkdir -p $DATADIR;
fi

/usr/local/dcd_protocol/bin/dcdnode \
--signature-provider DCD6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV=KEY:5KQwrPbwdL6PhXujxW37FSSQZ1JiwsST4cqQzDeyXtP79zkvFD3 \
--plugin dcd::producer_plugin \
--plugin dcd::producer_api_plugin \
--plugin dcd::chain_plugin \
--plugin dcd::chain_api_plugin \
--plugin dcd::http_plugin \
--plugin dcd::history_api_plugin \
--plugin dcd::history_plugin \
--data-dir $DATADIR"/data" \
--blocks-dir $DATADIR"/blocks" \
--config-dir $DATADIR"/config" \
--producer-name dcd \
--http-server-address 0.0.0.0:8888 \
--p2p-listen-endpoint 0.0.0.0:9010 \
--access-control-allow-origin='*' \
--contracts-console \
--http-validate-host=false \
--verbose-http-errors \
--enable-stale-production \
--p2p-peer-address 172.19.0.3:9011 \
--p2p-peer-address 172.19.0.4:9012 \
--p2p-peer-address 172.19.0.5:9013 \
--p2p-peer-address 172.19.0.6:9014 \
>> $LOGDIR"/dcdnode.log" 2>&1 & \
echo $! > $DATADIR"/dcdd.pid"