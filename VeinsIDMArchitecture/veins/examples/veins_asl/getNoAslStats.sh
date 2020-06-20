#!/bin/bash
pcregrep -M "Simulation ended.*(\n|.)*]]></stdout>" /home/anthl10/Programming/omnetpp-5.5.1/samples/veins/results_log.txt >> ./PerformanceNoASL/stats.txt
