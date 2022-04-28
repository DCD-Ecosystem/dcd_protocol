#!/bin/bash
DATADIR="/node/blockchain"
LOGDIR="/node/log"

if [ ! -d $DATADIR ]; then
  mkdir -p $DATADIR;
fi
if [ ! -d $LOGDIR ]; then
  mkdir -p $LOGDIR;
fi

/usr/local/dcd_protocol/bin/dcdnode \
--genesis-json $DATADIR"/../config/genesis.json" \
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
--p2p-listen-endpoint 0.0.0.0:9001 \
--access-control-allow-origin=* \
--contracts-console \
--http-validate-host=false \
--verbose-http-errors \
--enable-stale-production \
--p2p-peer-address 172.19.0.2:9010 \
--p2p-peer-address 172.19.0.3:9011 \
--p2p-peer-address 172.19.0.4:9012 \
--p2p-peer-address 172.19.0.5:9013 \
--p2p-peer-address 172.19.0.6:9014 \
>> $LOGDIR"/dcdnode.log" 2>&1 & \
echo $! > $DATADIR"/dcdd.pid"

touch $DATADIR/inited.lock