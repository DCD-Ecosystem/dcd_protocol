#!/bin/bash
DATADIR="/node/workblockchain"
CONFIGDIR="/node/config/work"

kdcdd --config-dir /node/dcd_wall --data-dir /node/dcd_wall --http-max-response-time-ms 99999 2> /node/dcd_wall/wdlog.txt &

if [ ! -f $DATADIR/inited.lock ]; then
    echo "runing genesis_start..."
    $CONFIGDIR/genesis_start.sh        
else 
    if [ ! -f $DATADIR/dcdd.pid ]; then
        echo "runing start.sh..."  
        $CONFIGDIR/start.sh
    else
        echo "runing hard_replay..."
        rm -rf $DATADIR/dcdd.pid
        $CONFIGDIR/hard_replay.sh
    fi
fi