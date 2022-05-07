#!/bin/bash
DATADIR="/node/blockchain"
CONFDIR="/node/config"

#echo privkey = $PRIV_KEY
echo pub = $PUB_KEY
echo acc = $ACC_NAME

if [ -f $CONFDIR/config_ready.lock ]; then
  if [ ! -f $DATADIR/inited.lock ]; then
    echo "runing genesis_start..."
    $CONFDIR/genesis_start.sh
  else
    if [ ! -f $DATADIR/dcdd.pid ]; then
        echo "runing start.sh..."
        $CONFDIR/start.sh
    else
        echo "runing hard_replay..."
        rm -rf $DATADIR/dcdd.pid
        $CONFDIR/hard_replay.sh
    fi
  fi
else
   echo "runing generate config files..."
   listfiles="genesis_start.sh hard_replay.sh start.sh"
#    listfiles="genesis_start.sh"
    for filename in $listfiles
    do
        #passfile=$wddir'/'$accname'pass.txt';
        #pubacckey=`grep "^Public key:" $passfile | tail -1 | sed "s/^Public key://"`
        #priacckey=`grep "^Private key:" $passfile | tail -1 | sed "s/^Private key://"`
        cat $CONFDIR/$filename.in | sed -e "s/PUB_KEY/$PUB_KEY/;s/:PRIV_KEY/:$PRIV_KEY/;s/ACC_NAME/$ACC_NAME/" | sed "s/KEY: /KEY:/" > $CONFDIR/$filename
        chmod a+x $CONFDIR/$filename
    done
   touch $CONFDIR/config_ready.lock
fi
