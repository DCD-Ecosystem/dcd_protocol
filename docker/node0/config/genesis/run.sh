#!/bin/bash
DATADIR="/node/blockchain"
CONFDIR="/node/config"
GENSGDIR="$CONFDIR/genesis"
INITDIR="$CONFDIR/inited"
WORKDATA="/node/workblockain"
WORKCONFIG="$CONFDIR/work"


if [ ! -f $GENSGDIR/desabled.lock ]; then

    if [ ! -f $DATADIR/inited.lock ]; then
	echo "runing genesis_start..."
	$GENSGDIR/genesis_start.sh
	if [ ! -f $CONFDIR/config_created.lock ]; then
    	    echo "runing generate configs.."
    	    $INITDIR/genesis.sh
    	    touch $CONFDIR/config_created.lock
	fi    
    else 
	if [ ! -f $DATADIR/dcdd.pid ]; then
    	    echo "runing start.sh..."  
    	    $GENSGDIR/start.sh
	else
    	    echo "runing hard_replay..."
    	    rm -rf $DATADIR/dcdd.pid
    	    $GENSGDIR/hard_replay.sh
	fi
    fi
else 
    echo "running work node..."
    echo $($WORKCONFIG/run.sh)
fi