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

#include "veins/modules/application/ieee80211p/DemoBaseApplLayer.h"
#include "veins/modules/messages/ASLMessage_m.h"
#include "veins/modules/application/traci/ASLBSM_m.h"
#include "veins/modules/world/traci/trafficLight/TraCITrafficLightInterface.h"
//using namespace omnetpp;

namespace veins {

/**
 * @brief
 * This is a stub for a typical Veins application layer.
 * Most common functions are overloaded.
 * See ASLApp.cc for hints
 *
 * @author David Eckhoff
 *
 */

class VEINS_API ASLAppRSUAttack : public DemoBaseApplLayer {
public:
    void initialize(int stage) override;
    void initialize(int stage, int deviceType);
    void finish() override;

protected:
    int deviceType;
    int receivedASL=0;
    int receivedASLBSM=0;
    int generatedASLs=0;
    int vehiclesInFront = 0; // will need to update this according to a loop detector around 300m away from intersection and one at intersection
    // if car reaches loop detector away from intersection, we increase
    // if car reaches loop detector at intersection, we decrease (they left)

    TraCITrafficLightInterface* tlsInterface;
    TraCICommandInterface* traci;
    TraCICommandInterface::Trafficlight* tlsTraci;

    void onBSM(DemoSafetyMessage* bsm) override;
    void onWSM(BaseFrame1609_4* wsm) override;
    void onWSA(DemoServiceAdvertisment* wsa) override;
    void onASL(ASLMessage* asl);
    void onASLBSM(ASLBSM* aslBsm);
    void populateASL(ASLMessage* asl, LAddress::L2Type senderId, LAddress::L2Type rcvId);
    void populateWSM(BaseFrame1609_4* wsm, LAddress::L2Type senderId, LAddress::L2Type rcvId);
    void checkAndTrackPacket(cMessage* msg) override;
    void handleLowerMsg(cMessage* msg) override;
    void handleSelfMsg(cMessage* msg) override;
    void handlePositionUpdate(cObject* obj) override;

    /* ATTACK MODELS */
    // TODO : will take ASL packet or ASLBSM packet as input and modify the variables according to the attack model
    void commDelayAttack(){};
    void gpsAttack(){};
    void speedAttack(){};
    void trafficSignalAttack(){};
    void inductionLoopAttack(){};
};

} // namespace veins
