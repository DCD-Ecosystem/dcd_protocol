#!/bin/bash
echo run docker build...
docker build -f Dockerfile_dcd_protocol -t dcd_prot_img . \
>> "docker_build.log" 2>&1 & \
echo $! > "docker_build.pid"
tail -f docker_build.log
