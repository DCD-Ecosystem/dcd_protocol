#!/bin/bash
curdir=$(dirname $(realpath "0"))
cd $curdir
logfile=$curdir"/log/dcdnode.log"

if [ -f $logfile ]; then
    tail -f $logfile
else
    echo $logfile not found! \n exit...
fi
