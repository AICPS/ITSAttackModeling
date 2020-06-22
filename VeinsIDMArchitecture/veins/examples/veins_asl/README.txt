SUMO Route/Trace XML Files:
In this directory, there are multiple configurations for SUMO traces (.rou.xml files) according to the network density you may wish to test with. We only provide traces for N=10, 20, 30, ... 120 here (128 is considered jam density with no cars able to move). Each configuration file contains the car-following model of user choice and its corresponding user-defined parameters. We are using the IDM model, but you may change it according to your choice of car-following model (please see SUMO documentation for more detail).

To make sure that the SUMO configuration files are using the correct route xml files, you may use the change_N.sh script to do so. This script will also change some of the header files used in our simulation tool.


-----------------------------------------------------------------------------
Running the Simulation Tool:  
