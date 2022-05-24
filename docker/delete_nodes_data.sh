#!/bin/bash
curdir=$(dirname $(realpath "0"))
cd $curdir
sudo rm -rf $curdir/blockchain/
sudo rm -rf $curdir/log/
sudo rm -rf $curdir/config/config_ready.lock
sudo rm -rf $curdir/config/genesis_start.sh
sudo rm -rf $curdir/config/hard_replay.sh
sudo rm -rf $curdir/config/start.sh
sudo rm -rf $curdir/config/wallet
