#!/bin/bash
curdir=$(dirname $(realpath "0"))
cd $curdir

if [ -f $curdir"/docker_build.pid" ]; then
    pid=`cat $curdir"/docker_build.pid"`
    echo $pid
    echo "2s"
    sudo kill $pid
    rm -r $curdir"/docker_build.pid"
    echo -ne "Stoping docker build"
    while true; do
        [ ! -d "/proc/$pid/fd" ] && break
        echo -ne "."
        sleep 1
    done
    echo -ne "\rDocker build Stopped. \n"
fi
