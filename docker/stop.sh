#!/bin/bash
curdir=$(dirname $(realpath "0"))
cd $curdir
echo stopping nodes ... 
sudo docker-compose down
