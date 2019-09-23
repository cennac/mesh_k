#!/bin/sh

pids=$(ps -ef | grep boa | grep -v grep | grep -v $0 | awk '{print $1}')
for pid in $pids
do
    kill -9 $pid
done

if [ "$1" = "0" ];then
boa &
else
boa -f /etc/boa/boa_rtk.conf &
fi