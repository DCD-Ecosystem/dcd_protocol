#!/bin/bash
curdir=$(dirname $(realpath "0"))
cd $curdir

if [ -f $curdir"/docker_build.log" ]; then
    tail -f $curdir"/docker_build.log"
else
    echo $curdir"/docker_build.log" not found!
fi
