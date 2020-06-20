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

#include "veins/modules/application/traci/ASLAppRSUAttack.h"


using namespace veins;

Define_Module(veins::ASLAppRSUAttack);

void ASLAppRSUAttack::initialize(int stage)
{
    DemoBaseApplLayer::initialize(stage);
    if (stage == 0) {
        // Initializing members and pointers of your application goes here
        EV << "Initializing " << par("appName").stringValue() << std::endl;

        // initialize pointers to other modules
        if (FindModule<TraCITrafficLightInterfaceAccess*>::findSubModule(getParentModule())) {
            tlsInterface = TraCITrafficLightInterfaceAccess().get(getParentModule());
            tlsInterface->setExternalId("gneJ0");
            traci = tlsInterface->getCommandInterface();
            tlsTraci = tlsInterface->getTlCommandInterface();
        }
        else {
            traci = nullptr;
        }
    }
    else if (stage == 1) {
        // Initializing members that require initialized other modules goes here
    }

    //tlsInterface = new TraCITrafficLightInterface();
    //tlsInterface->preInitialize();
}

void ASLAppRSUAttack::initialize(int stage, int deviceType){
    // deviceType = 0 -> vehicle
    // deviceType = 1 -> RSU
    initialize(stage);
    this->deviceType = deviceType;
}

void ASLAppRSUAttack::finish()
{
    DemoBaseApplLayer::finish();
    // statistics recording goes here
}

void ASLAppRSUAttack::onBSM(DemoSafetyMessage* bsm)
{

}

void ASLAppRSUAttack::onWSM(BaseFrame1609_4* wsm)
{
    // Your application has received a data message from another car or RSU
    // code for handling the message goes here, see TraciDemo11p.cc for examples
}

void ASLAppRSUAttack::onWSA(DemoServiceAdvertisment* wsa)
{
    // Your application has received a service advertisement from another car or RSU
    // code for handling the message goes here, see TraciDemo11p.cc for examples

}

void ASLAppRSUAttack::onASL(ASLMessage* asl)
{
    // Your application has received an ASL packet from an RSU
    // code for handling the message goes here, see TraciDemo11p.cc for examples
}

void ASLAppRSUAttack::onASLBSM(ASLBSM* aslBsm)
{
    // Your application has received an ASL packet from an RSU
    // code for handling the message goes here, see TraciDemo11p.cc for examples
    // Your application has received a beacon message from another car or RSU
    // code for handling the message goes here

    // if this car is an RSU and it receives a BSM packet, then it will determine if it should send an ASL packet to the vehicle
    // it will check if that vehicle is past the loop detector?

    //

   /* if(this->deviceType != 1)
        return; */

    // here the RSU will get the BSM and compute the appropriate ASL velocity if the vehicle is within the communication range
    // with heading toward the current intersection ****

    // need to access the traffic light parameters here as well
    // the name of the traffic light signal/junction will be different in each network xml. so need to be careful about that.
    simtime_t timeLeft = tlsInterface->getRemainingDuration();
   /* TraCITrafficLightProgram tlProgram = tlsTraci->getProgramDefinition();
*/
    // get state and phase and check how much time until the next phase/cycle
  /*  TraCITrafficLightProgram::Phase phase = tlProgram.Phase; */
    TraCITrafficLightProgram::Phase phase = tlsInterface->getCurrentPhase();
    std::cout << "The current phase state is: " << phase.state << std::endl;
    std::cout << "The remaining time is : " << timeLeft.str() << std::endl;

    // check if the heading is correct / or just check that the position is within the expected range
    // since coordinates are being used, we may need to have a coordinate range that satisfies this range

    // get their speed and calculate their arrival time

    // need to get the estimated number of vehicles in front
    // using data from the induction loop

    // arrival time = min(t + (L - L_int - position(t))/vf, tcurr + 3600* vehicles_in_front / service rate)
    // service rate = 1800/3600 vehicles per second

    // get the address of the vehicle so that we can send ASL message to it

    LAddress::L2Type senderId = aslBsm->getSenderAddress();
    Coord position = aslBsm->getSenderPos();
    Coord speed = aslBsm->getSenderSpeed();

    // time at which the packet arrives to RSU is different than when vehicle sent the packet
    // so need to adjust accordingly to possible delay
    simtime_t currTime = simTime();

    // here we will construct an ASL message to send to the vehicle
    ASLMessage* asl = new ASLMessage();
    LAddress::L2Type myId = mac->getMACAddress();
    //ASLAppRSUAttack::populateWSM(asl, myId, senderId);
    ASLAppRSUAttack::populateASL(asl, myId, senderId);
    sendDown(asl);
    scheduleAt(simTime(), sendBeaconEvt);
    //populateASL(asl);
}

void ASLAppRSUAttack::populateASL(ASLMessage* asl, LAddress::L2Type senderId, LAddress::L2Type rcvId){
    asl->setRecipientAddress(rcvId);
    asl->setBitLength(headerLength);
    asl->setPsid(-1);
    asl->setChannelNumber(static_cast<int>(Channel::cch));
    asl->addBitLength(beaconLengthBits);
    asl->setUserPriority(beaconUserPriority);
}

void ASLAppRSUAttack::populateWSM(BaseFrame1609_4* wsm, LAddress::L2Type senderId, LAddress::L2Type rcvId){
    wsm->setRecipientAddress(rcvId);
    wsm->setBitLength(headerLength);
    if (ASLMessage* asl = dynamic_cast<ASLMessage*>(wsm)) {
        //aslBsm->setSenderAddress(senderId);
        //aslBsm->setSenderPos(curPosition);
        //aslBsm->setSenderSpeed(curSpeed);
        asl->setPsid(-1);
        asl->setChannelNumber(static_cast<int>(Channel::cch));
        asl->addBitLength(beaconLengthBits);
        asl->setUserPriority(beaconUserPriority);
    }else {
        DemoBaseApplLayer::populateWSM(wsm);
    }
}

void ASLAppRSUAttack::handleLowerMsg(cMessage* msg){
    BaseFrame1609_4* wsm = dynamic_cast<BaseFrame1609_4*>(msg);
    ASSERT(wsm);

    // asl message is a unique message for this application so we do not include it in the base demo app layer
    if (ASLMessage* asl = dynamic_cast<ASLMessage*>(wsm)) {
        receivedASL++;
        //onASL(asl);
            // ignore asl messages if RSU
    }else if(ASLBSM* aslBsm = dynamic_cast<ASLBSM*>(wsm)){
        receivedASLBSM++;
        onASLBSM(aslBsm);
    }else{
        DemoBaseApplLayer::handleLowerMsg(msg);
    }
}

void ASLAppRSUAttack::handleSelfMsg(cMessage* msg)
{

    //DemoBaseApplLayer::handleSelfMsg(msg);
    // this method is for self messages (mostly timers)
    // it is important to call the DemoBaseApplLayer function for BSM and WSM transmission

}

void ASLAppRSUAttack::handlePositionUpdate(cObject* obj)
{
    DemoBaseApplLayer::handlePositionUpdate(obj);
    // the vehicle has moved. Code that reacts to new positions goes here.
    // member variables such as currentPosition and currentSpeed are updated in the parent class
}
void ASLAppRSUAttack::checkAndTrackPacket(cMessage* msg)
{
    if(dynamic_cast<ASLMessage*>(msg)){
        EV_TRACE << "sending down an ASL" << std::endl;
        generatedASLs++;
    }else {
        DemoBaseApplLayer::checkAndTrackPacket(msg);
    }
}
