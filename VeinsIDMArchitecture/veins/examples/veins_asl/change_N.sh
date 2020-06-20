#!/bin/bash
newN=$1
sed -i -e 's/veh_.*.rou.xml/veh_'$newN'.rou.xml/g' manhattan_asl.sumo.cfg
sed -i -e 's/veh_.*.rou.xml/veh_'$newN'.rou.xml/g' manhattan_asl.launchd.xml
pushd ../../src/veins/modules/application/traci/;
sed -i -e 's/N=.*;/N='$newN';/g' ASLAppRSU.h
sed -i -e 's/N=.*;/N='$newN';/g' ASLAppVeh.h
