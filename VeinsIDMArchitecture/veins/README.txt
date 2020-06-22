Installation:
See VEINS_TOOLS_VERSIONS_SETUP file for the versions of the Veins, SUMO, and OM
NET++ used. 
Paths to ITS/ASL security related code are: VeinsIDMArchitecture/veins/examples
/veins_asl/ and VeinsIDMArchitecture/veins/src/veins/modules/messages

Once you install your own Veins-related software, you may use the code that we provide here in the "examples" directory that comes with Veins by default. 
We have our own customized ASL messages, SUMO configuration files, network files, etc. 

Simulation:
In this directory, in addition to using the default script sumo-launchd.py to run SUMO with Veins.
We have provided a script sumo_script.sh that will output valuable performance metrics to results_log.txt 
(which eases the difficulties of finding the latest results in the /tmp/ directory). 

Results_Log.txt Interpretation (also see SUMO documentation):
The metrics that we used in our work are as follows:
Average Network Flow Rate: (Average Trip Duration / Average Trip Length) * (N/L) ; 
where N is the average number of vehicles in the network and L is the road length (we use L=900 m);
we normalize the average network flow rate with the corresponding theoretical Capacity flow rate for the Car Following Model
(for IDM it is about 0.4).
Average Trip Waiting Ratio: Average Trip Waiting Time / Average Trip Duration  
we also normalize this value with the waiting ratio of the simulation scenario when there is No ASL. 

---------------------------------------------------------------------------------------
Veins - The open source vehicular network simulation framework.

See the Veins website <http://veins.car2x.org/> for a tutorial, documentation,
and publications.

Veins is composed of many parts. See the version control log for a full list of
contributors and modifications. Each part is protected by its own, individual
copyright(s), but can be redistributed and/or modified under an open source
license. License terms are available at the top of each file. Parts that do not
explicitly include license text shall be assumed to be governed by the "GNU
General Public License" as published by the Free Software Foundation -- either
version 2 of the License, or (at your option) any later version
(SPDX-License-Identifier: GPL-2.0-or-later). Parts that are not source code and
do not include license text shall be assumed to allow the Creative Commons
"Attribution-ShareAlike 4.0 International License" as an additional option
(SPDX-License-Identifier: GPL-2.0-or-later OR CC-BY-SA-4.0). Full license texts
are available with the source distribution.

