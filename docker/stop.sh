#!/bin/bash
curdir=$(dirname $(realpath "0"))
cd $curdir
echo stopping nodes ... 
sudo docker exec -it usernode /node/config/stop.sh
sudo docker-compose down
