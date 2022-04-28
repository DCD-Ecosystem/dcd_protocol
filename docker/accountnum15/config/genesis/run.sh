#!/bin/bash
DATADIR="/node/blockchain"
CONFIGDIR="/node/config/genesis"

echo $(hostname -i) > $CONFIGDIR/ipaddr

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
