#!/bin/bash
#pcregrep -M "Simulation ended.*(\n|.)*]]></stdout>" /home/anthl10/Programming/omnetpp-5.5.1/samples/veins/results_log.txt >> ./PerformanceASL/stats_attack.txt
cp /home/anthl10/Programming/omnetpp-5.5.1/samples/veins/results_log.txt ./PerformanceASL/stats_attack.txt
echo "Do not forget to rename the file according to attack and attacker type"
