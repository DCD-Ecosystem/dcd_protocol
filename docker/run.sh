#!/bin/bash
BASEDIR=$(dirname $(realpath "$0"))
NODEDIR="$BASEDIR/node0"
CONFIGDIR="$NODEDIR/config"

cd $BASEDIR
if [ -f $CONFIGDIR/genesis/desabled.lock ]; then
    sudo docker-compose up -d
else 

sudo docker-compose up node0 -d

echo waiting for generate config files..
while [ ! -f $CONFIGDIR/config_created.lock ]
do
    echo -n "."
    sleep 1
done

echo copy config files..
listfiles="genesis_start.sh hard_replay.sh start.sh"
createacclist="accountnum11 accountnum12 accountnum13 accountnum14 accountnum15"
for createacc in $createacclist
do

    echo "createacc: $createacc"
    
    for filename in $listfiles
    do
    
        echo "filename: $filename"
        
        srcfile=$CONFIGDIR/$createacc/$filename;        
        dstfile=$BASEDIR/$createacc/config/genesis/$filename
        echo "srcfile: $srcfile"
        echo "dstfile: $dstfile"
        cp -f $srcfile $dstfile
        chmod a+x $dstfile
    done
done

echo run nodes
sudo docker-compose up -d
fi
