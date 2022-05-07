#!/bin/bash
ACCOUNTNAME=""
PUBLICKEY=""
PRIVATEKEY=""

BASEDIR=$(dirname $(realpath "$0"))
CONFIGDIR="$BASEDIR/config"

cd $BASEDIR
doexit=0
if [ ! $ACCOUNTNAME ]; then
   echo Enter a ACCOUNTNAME!
   doexit=1
fi
if [ ! $PUBLICKEY ]; then
   echo Enter a PUBLICKEY!
   doexit=1
fi
if [ ! $PRIVATEKEY ]; then
   echo Enter a PRIVATEKEY!
   doexit=1
fi

if [ $doexit -eq 1 ]; then
   echo Exit!
   exit 1
fi

if [ ! -f $CONFIGDIR/config_ready.lock ]; then
  sudo docker-compose create node
  sudo docker-compose run --rm -e ACC_NAME=$ACCOUNTNAME -e PRIV_KEY=$PRIVATEKEY -e PUB_KEY=$PUBLICKEY node /node/config/run.sh

  echo waiting for generate config files..
  while [ ! -f $CONFIGDIR/config_ready.lock ]
  do
    echo -n "."
    sleep 1
  done

  sudo docker-compose stop node
fi

sudo docker-compose up -d
echo Node is work.
