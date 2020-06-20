//
// Copyright (C) 2016 David Eckhoff <david.eckhoff@fau.de>
//
// Documentation for these modules is at http://veins.car2x.org/
//
// SPDX-License-Identifier: GPL-2.0-or-later
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

#pragma once

#include "veins/veins.h"
#include "veins/modules/application/traci/ASLBSM_m.h"
#include "veins/modules/application/ieee80211p/DemoBaseApplLayer.h"
#include "veins/modules/messages/ASLMessage_m.h"
//#include "veins/modules/application/traci/RSUAttack.h"
//#include "veins/modules/application/traci/ASLAppRSU.h"

//using namespace omnetpp;

namespace veins {

/**
 * @brief
 * This is a stub for a typical Veins application layer.
 * Most common functions are overloaded.
 * See ASLAppRSU.cc for hints
 *
 * @author David Eckhoff
 *
 */

class VEINS_API ASLAppVeh : public DemoBaseApplLayer {
public:
    ~ASLAppVeh() override;
    void initialize(int stage) override;
    void initialize(int stage, int deviceType);
    void finish() override;

protected:
    bool logging=false;
    int deviceType;
    int receivedASL=0;
    int generatedASLBSM=0;
    int receivedASLBSM=0;
    double lastAccWaitTime = 0.0;
    double lastEndTime = 0.0;
    double totalAccWaitTime = 0.0;
    double startTime = 0.0;
    bool startTimeSet = false;
    bool sent = false;
    bool vehicleSent = false;
    double speedLimit = 15.0;
    int N=50;
    bool onASLBool=true;
    double T = 60.0;
    double initTime = 10.0 * T;
    double waitTimeThreshold = 0.1;
    int count = 0;
    bool slowDownSet =false;
    bool receivedAsl = false;
    double estArrivalTime = 0.0;
    Coord* interPos = new Coord(50,900);
    void onBSM(DemoSafetyMessage* bsm) override;
    void onWSM(BaseFrame1609_4* wsm) override;
    void onWSA(DemoServiceAdvertisment* wsa) override;
    void onASL(ASLMessage* asl);
    bool checkIfQueue(std::string laneId);
    bool ewJunctionBusy();
    bool removeVirtualCar();
    void populateWSM(ASLBSM* aslBsm, LAddress::L2Type senderId, LAddress::L2Type rcvId);
    void checkAndTrackPacket(cMessage* msg) override;
    void handleLowerMsg(cMessage* msg) override;
    void handleSelfMsg(cMessage* msg) override;
    void handlePositionUpdate(cObject* obj) override;
    double comm_delay = 1.5;


    double randomSelect(double first, double second){
        // can choose what kind of random distribution  here as well. we make all values fair for now
        return uniform(first, second, 0);
    }

    bool commAttackOn = false;
    bool isRandomTimeAttack = true; // flag to determine if the moment of attack is chosen at random each time, or static / deterministic
    bool isPeriodicAttack = false; // flag to indicate that attack will occur periodically with a specific duration rather than the entire time
    bool isRandomValAttack = true; // flag to determine if the value of attack is chosen at random or not
    double randomTimeFreq = 1.0; // how frequently the random value is changed; two dimensions: time and cars
    double randomCarFreq = 0.4; // which cars are attacked with this model
    double attackStartTime = 0.0; // start time of the attack
    double attackEndTime = 0.0; // end time of the attack -> endtime - starttime = duration
    double attackFrequency = 0.0;
    int impactLevel = 1; // 0, 1, 2 -> stealthy, medium, heavy ; correspond to different levels of impact based on the targeted variable
    double attackPeriod = 30.0;
    double attackOffset = 30.0;
    double minPerturb = 0.0;
    double maxPerturb = 1.0; // this is the range of perturbation that is permitted and based on these and the impact level, the final value is selected
    int attackRandSeed = 0;

    // TODO : will take ASL packet or ASLBSM packet as input and modify the variables according to the attack model
    /* Targets the communication delay of the packets either sent to the RSU or sent back to the car */
    // we focus on delay of packet to RSU first
    double commDelayAttack(double origVal, std::map<long, double>* expArrivalTimes, LAddress::L2Type senderId){
        double attackVal = 0.0;

        if(impactLevel==0){
            minPerturb = comm_delay;
            maxPerturb = 2.0*comm_delay;
        }
        else if(impactLevel==1){
            minPerturb = 2.0*comm_delay;
            maxPerturb = 5.0*comm_delay;
        }else{
            minPerturb = 5.0*comm_delay;
            maxPerturb = 10.0*comm_delay; //really high delay
        }

        if(isRandomValAttack){
            attackVal = origVal + randomSelect(minPerturb, maxPerturb);
        }else{
            attackVal = maxPerturb;
        }
        return attackVal;
    };

};

} // namespace veins
