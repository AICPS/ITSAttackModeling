#!/bin/bash
rm results_log.txt
./sumo-launchd.py -vv -c sumo 2> results_log.txt
