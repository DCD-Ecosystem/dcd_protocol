#!/bin/bash
DATADIR="/node/workblockchain"

if [ -f $DATADIR"/dcdd.pid" ]; then
    pid=`cat $DATADIR"/dcdd.pid"`
    echo $pid
    echo "2s"
    kill $pid
    rm -r $DATADIR"/dcdd.pid"
    echo -ne "Stoping Node"
    while true; do
        [ ! -d "/proc/$pid/fd" ] && break
        echo -ne "."
        sleep 1
    done
    echo -ne "\rNode Stopped. \n"
fi
