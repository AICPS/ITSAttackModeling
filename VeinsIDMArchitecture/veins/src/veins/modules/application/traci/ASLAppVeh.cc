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

#include <veins/modules/application/traci/ASLAppVeh.h>

using namespace veins;

Define_Module(veins::ASLAppVeh);

void ASLAppVeh::initialize(int stage)
{
    DemoBaseApplLayer::initialize(stage);
    if (stage == 0) {
        // Initializing members and pointers of your application goes here
        EV << "Initializing " << par("appName").stringValue() << std::endl;
    }
    else if (stage == 1) {
        // Initializing members that require initialized other modules goes here
    }
    if(!startTimeSet){
        startTimeSet = true;
        startTime = simTime().dbl();
        traciVehicle->setMaxSpeed(speedLimit);
    }else{
        traciVehicle->setMaxSpeed(speedLimit);
    }
}

void ASLAppVeh::initialize(int stage, int deviceType){
    // deviceType = 0 -> vehicle
    // deviceType = 1 -> RSU
    initialize(stage);
    this->deviceType = deviceType;
}

void ASLAppVeh::finish()
{
//    double time = simTime().dbl();
//    std::string filename = "NoASL_PerformanceResults_N";
//    filename.append(std::to_string((int)(N)));
//    filename.append(".txt");
//    std::ofstream outputFile(filename);
    double time = simTime().dbl();
//    outputFile << "Ending time for this car is: " << time << std::endl;
//    outputFile << "Last position is: " << curPosition << std::endl;
       /*
    std::cout << "Ending time for this car is: " << time << std::endl;
    std::cout << "Last position is: " << curPosition << std::endl;
    }*/

    /* We don't care  about this car unless it comes after initial time mark - for stability purposes */
    if(time > initTime){

      //  std::cout << "Total waiting time percentage for this car " << traciVehicle->getVehicleId() <<  " is: " << lastAccWaitTime/(time - startTime) << std::endl;
        traci->setNumCars(traci->getNumCars()+1);
        double tempTotAccTime = 0.0;
        if( lastAccWaitTime/(time - std::max(startTime, initTime)) > 1) {
            tempTotAccTime = time-std::max(startTime, initTime);
       }else{
            tempTotAccTime = lastAccWaitTime/(time - std::max(startTime, initTime));
        }
       // std::cout << "Car: " << nodeId << std::endl;
  /*      std::cout << "Time is: " << time;
        std::cout << " startTime is: " << std::max(startTime, initTime) << std::endl;
        std::cout << "Last acc wait time is: " << lastAccWaitTime << std::endl;
        std::cout << "Temp tot acc wait time: " << tempTotAccTime << std::endl;
*/
        traci->setTotalWaitTime((traci->getTotalWaitTime() + tempTotAccTime));
        if(time > 1199){
            std::string filename;
            if(onASLBool){
              //  std::cout << "ASL is on\n";
                filename = "WaitLogic_ASL_VehResults_N";
            }else{
                //std::cout << "ASL is off\n";
                filename = "WaitLogic_NoASL_VehResults_N";
            }
            filename.append(std::to_string((int)(N)));
            filename.append("_thresh_");
            filename.append(std::to_string((double)(waitTimeThreshold)));
            filename.append(".txt");
      //      std::ofstream outputFile(filename);
      //      outputFile << "Total acc waiting time of all cars is: " << traci->getTotalWaitTime() << std::endl;
           // outputFile << "Start time: " << startTime << " and end time : " << time << " and last acc wait time " << lastAccWaitTime << std::endl;
      //      outputFile << "Number of cars: " << traci->getNumCars() << std::endl;
      //      outputFile << "Averaged waiting time ratio is: " << (traci->getTotalWaitTime()/traci->getNumCars()) << std::endl;
  //     std::cout << "Number of cars: " << traci->getNumCars() << std::endl;
 //     std::cout << "Waiting Time Threshold is: " << waitTimeThreshold << std::endl;
 //       std::cout << "Total waiting time " << (traci->getTotalWaitTime()) << std::endl;
//            std::cout << "Averaged waiting time ratio is: " << (traci->getTotalWaitTime()/traci->getNumCars()) << std::endl;

      //      outputFile.close();
        }
    }



    DemoBaseApplLayer::finish();
    // statistics recording goes here
    recordScalar("VehgeneratedASLBSMs", generatedASLBSM);
    recordScalar("VehreceivedASLBSMs", receivedASLBSM);
    recordScalar("VehreceivedASLs", receivedASL);


}

void ASLAppVeh::onBSM(DemoSafetyMessage* bsm)
{
    // Your application has received a beacon message from another car or RSU
    // code for handling the message goes here

    // if this car is an RSU and it receives a BSM packet, then it will determine if it should send an ASL packet to the vehicle
    // it will check if that vehicle is past the loop detector?

}

void ASLAppVeh::onWSM(BaseFrame1609_4* wsm)
{
    // Your application has received a data message from another car or RSU
    // code for handling the message goes here, see TraciDemo11p.cc for examples
    //TraCIDemo11pMessage* wsm = check_and_cast<TraCIDemo11pMessage*>(frame);

    // this rsu repeats the received traffic update in 2 seconds plus some random delay
 /*   sendDelayedDown(wsm->dup(), 2 + uniform(0.01, 0.2)); */
}

void ASLAppVeh::onWSA(DemoServiceAdvertisment* wsa)
{
    // Your application has received a service advertisement from another car or RSU
    // code for handling the message goes here, see TraciDemo11p.cc for examples
    // if this RSU receives a WSA for service 42, it will tune to the chan
   /* if (wsa->getPsid() == 42) {
        mac->changeServiceChannel(static_cast<Channel>(wsa->getTargetChannel()));
    } */

}

void ASLAppVeh::onASL(ASLMessage* asl)
{
    // Your application has received an ASL packet from an RSU
    // code for handling the message goes here, see TraciDemo11p.cc for examples
    if(this->myId != asl->getRecipientAddress())
       return;
    double newMaxSpeed = asl->getAslSpeed();
    ////// ENABLE / DISABLE ASL APPLICATION
    if(onASLBool && traciVehicle->getLaneId() == "H4G4_0" ){
        traciVehicle->setMaxSpeed(newMaxSpeed);
    }else{
        traciVehicle->setMaxSpeed(speedLimit);
    }
}

void ASLAppVeh::handleLowerMsg(cMessage* msg){
    BaseFrame1609_4* wsm = dynamic_cast<BaseFrame1609_4*>(msg);
    ASSERT(wsm);

    // asl message is a unique message for this application so we do not include it in the base demo app layer
    if (ASLMessage* asl = dynamic_cast<ASLMessage*>(wsm)) {
        receivedASL++;
        onASL(asl); // handle asl packet
        //delete(msg);
    }else if(dynamic_cast<ASLBSM*>(wsm)){
        receivedASLBSM++;
        //delete(msg);
        //delete(aslBsm);
    }else{
        //DemoBaseApplLayer::handleLowerMsg(msg); // other normal type of message
    }
    delete(msg);
}

void ASLAppVeh::handleSelfMsg(cMessage* msg)
{
    TraCICommandInterface::Lane* lane = new TraCICommandInterface::Lane(traci, "H4G4_0");
    ASLBSM* aslBsm = new ASLBSM();
    double time = simTime().dbl();
    switch (msg->getKind()) {
        case SEND_BEACON_EVT: {
            LAddress::L2Type senderId = mac->getMACAddress();
            ASLAppVeh::populateWSM(aslBsm, senderId, 0);
            double lanePos = traciVehicle->getLanePosition();
            double tcomm_delay = 1.5;
            std::time_t t;
            if(curDirection.x == -1.0 && traciVehicle->getLaneId() == "H4G4_0" ){
                if(logging){
                std::cout << "This cars " << traciVehicle->getVehicleId() << " position is: " << curPosition.x << std::endl;
                std::cout << "This cars " << traciVehicle->getVehicleId() << " lane position is: " << traciVehicle->getLanePosition() << std::endl;
                std::cout << "This cars " << traciVehicle->getVehicleId() << " lane id is: " << traciVehicle->getLaneId() << std::endl;
                }
                if(lanePos >= 600.0  && lanePos < 889.9){ // within DSRC range and not past intersection
                /////   std::cout << "VEH:: This EW vehicle will send an ASLBSM" << std::endl;
                /////   std::cout << "VEH::This vehicle " << traciVehicle->getVehicleId() << " current position is: " << mobility->getPositionAt(simTime()) << std::endl;
                /////   std::cout << "VEH::This vehicle " << traciVehicle->getVehicleId() << " curr direction is " << curDirection << std::endl;

                    if(onASLBool){
                        ///////TEST: commDelayAttack()
                        if(commAttackOn){
                            std::map<long, double>* expArrivalTimes = new std::map<long, double>();
                            if(isRandomTimeAttack){ ///if there is a random chance that this attack will occur at this time t, for this car
                                if(uniform(0.0, 1.0, 0) <= randomTimeFreq){
                                    std::srand((unsigned) (std::time) (&t));
                                    if(uniform(0.0, 1.0, attackRandSeed) <= randomCarFreq){ //if rand val > given threshold
                                        tcomm_delay = commDelayAttack(tcomm_delay, expArrivalTimes, senderId);
                                    }
                                }
                            }else if(isPeriodicAttack){///if there is a random chance that this attack will occur at this time t, for this car
                                if (std::fmod(time, attackPeriod + attackOffset) <= attackPeriod){
                                    tcomm_delay = commDelayAttack(tcomm_delay, expArrivalTimes, senderId);
                                }
                            }else{ // else, just perform attack
                                tcomm_delay = commDelayAttack(tcomm_delay, expArrivalTimes, senderId);
                            }
                        }

                        sendDelayedDown(aslBsm, tcomm_delay);
                        if(logging) std::cout << "Vehicle "<< traciVehicle->getVehicleId() << " sending packet at time " << simTime().dbl() << std::endl;
                    }
                }
            }
        }

        scheduleAt(simTime() + beaconInterval, sendBeaconEvt);
        break;
        default: {
            if (msg) EV_WARN << "APP: Error: Got Self Message of unknown kind! Name: " << msg->getName() << endl;
            break;
        }
    }

    if(((traciVehicle->getLaneId() == "H4G4_0" &&
            traciVehicle->getLanePosition() >= 885.0) || (
            traciVehicle->getLaneId()==":G4_4_0") )){
        if((traci->getVehicleIds().size() > 60 &&
                traci->getVehicleIds().size() < N
                && !checkIfQueue("H4G4_0")) ||
                checkIfQueue("H4G4_0")){
            traciVehicle->slowDown(std::max(traciVehicle->getSpeed()-15,
                    0.0), 1.5);
        }else{
            traciVehicle->setMaxSpeed(speedLimit);
        }
    }

    // ADD VEHICLE TO BEGINNING OF ROUTE ENTRANCE TO KEEP NUMBER OF VEHICLES THE SAME
    if((traciVehicle->getLaneId()=="G4F4_0" ||
            traciVehicle->getLaneId()==":G4_4_0") && !vehicleSent){
        std::string thisId = traciVehicle->getVehicleId();
        double thisIdDbl = std::stod(thisId);
        std::string vehicleId = std::to_string(thisIdDbl+N+1);
        std::string vehicleTypeId = "ASL_VTYPE";
        std::string routeId = traciVehicle->getRouteId();
        simtime_t emitTime_st = simTime()+1.0;
        double emitPosition = 0.0;
        double emitSpeed = traciVehicle->getSpeed();
        int8_t emitLane = 0;
        traci->addVehicle(vehicleId, vehicleTypeId,
                  routeId, emitTime_st, emitPosition, emitSpeed, emitLane);
        vehicleSent = true;
    }

}

void ASLAppVeh::populateWSM(ASLBSM* aslBsm, LAddress::L2Type senderId,
        LAddress::L2Type rcvId){
    //wsm->setRecipientAddress(rcvId);
    //wsm->setBitLength(headerLength);
        aslBsm->setSenderAddress(senderId);
        aslBsm->setVehId(traciVehicle->getVehicleId().c_str());
        aslBsm->setSenderPos(curPosition);
//        aslBsm->setSenderSpeed(curSpeed);
        aslBsm->setSenderSpeed(curDirection*mobility->getSpeed());
        aslBsm->setDirection(curDirection);
        //aslBsm->setHeading(curHeading);
        if(logging){
            std::cout << "VEH::This vehicle's current position is: " << mobility->getPositionAt(simTime()) << std::endl;
            std::cout << "VEH::This vehicle's current position is: " << curPosition << std::endl;
            std::cout << "VEH: This vehicle veins mac id is: " << ASLAppVeh::myId << std::endl;
            std::cout << "VEH: This vehicle sumo id is: " << traciVehicle->getVehicleId() << std::endl;
            std::cout << "VEH:: This car is going at speed " << mobility->getSpeed() << " and direction " << curDirection << std::endl;
            std::cout << "VEH:: Speed in terms of coord is: " << (Coord)(curDirection * mobility->getSpeed()) << std::endl;
            std::cout << "VEH:: Heading is: " << mobility->getHeading().toCoord() << std::endl;
        }
       //Heading* heading = new Heading(mobility->getHeading().getRad());
       // aslBsm->setHeading(*heading);
        aslBsm->setPsid(-1);
        aslBsm->setChannelNumber(static_cast<int>(Channel::cch));
        aslBsm->addBitLength(beaconLengthBits);
        aslBsm->setUserPriority(beaconUserPriority);
}

void ASLAppVeh::handlePositionUpdate(cObject* obj)
{
    DemoBaseApplLayer::handlePositionUpdate(obj);
    // the vehicle has moved. Code that reacts to new positions goes here.
    // member variables such as currentPosition and currentSpeed are updated in the parent class
}

void ASLAppVeh::checkAndTrackPacket(cMessage* msg)
{
    if(dynamic_cast<ASLBSM*>(msg)){
            EV_TRACE << "sending down an ASL" << std::endl;
            generatedASLBSM++;
    } else {
            DemoBaseApplLayer::checkAndTrackPacket(msg);
    }
}

bool ASLAppVeh::checkIfQueue(std::string laneId){
    TraCICommandInterface::Lane* currLane = new TraCICommandInterface::Lane(traci, laneId);
    std::list<std::string> carIds = currLane->getLastStepVehicleIds();
    TraCICommandInterface::Vehicle* veh;
    //std::cout << "This car's position is: " << traciVehicle->getLanePosition() << std::endl;
  //  while(!carIds.empty()){
        veh = new TraCICommandInterface::Vehicle(traci, carIds.front());
        //std::cout << "This car's position is: " << veh->getLanePosition() << std::endl;
        if(veh->getLanePosition() >= 0.0 && veh->getLanePosition()<=7.0 && veh->getSpeed() < 0.1){
            return true; // there is a queue at other side; cars at intersection should wait
        }else{
            return false;
        }
//      carIds.pop_front();
   // }
   // return false;
}

bool ASLAppVeh::ewJunctionBusy(){
    std::string laneId = "G4F4_0";
    TraCICommandInterface::Lane* currLane = new TraCICommandInterface::Lane(traci, laneId);
    std::list<std::string> carIds = currLane->getLastStepVehicleIds();
    TraCICommandInterface::Vehicle* veh;
    if(carIds.size() != 0){
        veh = new TraCICommandInterface::Vehicle(traci, carIds.front());
        if(veh->getLanePosition()<=7.0 &&
                veh->getSpeed() < 0.1){
            return true; // there is a queue at other side; cars at intersection should wait
        }else{
            return false;
        }
    }else{
        return false;
    }
}

bool ASLAppVeh::removeVirtualCar(){
    //std::string laneId = ":G4_4_0";
    std::string laneId = "G4F4_0";
    TraCICommandInterface::Lane* currLane =
            new TraCICommandInterface::Lane(traci, laneId);
    std::list<std::string> carIds = currLane->getLastStepVehicleIds();
    TraCICommandInterface::Vehicle* veh;
    while(carIds.size()>0){
        veh = new TraCICommandInterface::Vehicle(traci, carIds.front());
        if(veh->getVType() == "VIRTUAL"){
            //traci->removeVehicle(veh->getVehicleId());
            return true;
        }
        carIds.pop_front();
    }
    return false;

}

ASLAppVeh::~ASLAppVeh()
{
    cancelAndDelete(sendBeaconEvt);
    //cancelAndDelete(sendWSAEvt);
  //  findHost()->unsubscribe(BaseMobility::mobilityStateChangedSignal, this);
}




// slow down this car if there is a queue at other side
//    if(traciVehicle->getLaneId() == ":G4_4_0" ||
//            (traciVehicle->getLanePosition() > 880.0 &&
//             traciVehicle->getLanePosition() <=890.0)){
//        if(checkIfQueue("H4G4_0") ){
//            // each time step, can lower velocity by 3(1^2) m/s because of deceleration
//            traciVehicle->slowDown(std::max(traciVehicle->getSpeed()/2,0.0), 1.0);
//         /*   if(traciVehicle->getLanePosition() >= 888.0){
//                traciVehicle->slowDown(0.0, 1.0);
//            }
// */     }else if(traciVehicle->getLaneId() == ":G4_4_0"){
//            traciVehicle->setMaxSpeed(speedLimit);
//         }
//
//     }
// ADD VIRTUAL STOPPED CAR AT JUNCTION IF THERE IS QUEUE AT OTHER SIDE
/*
 if(checkIfQueue("H4G4_0")){
     if(!ewJunctionBusy()){
         std::string thisId = traciVehicle->getVehicleId();
         double thisIdDbl = std::stod(thisId);
         std::string vehicleId = std::to_string(thisIdDbl+N+1);
         std::string vehicleTypeId = "VIRTUAL";
         std::string routeId = "eastwestjunction";
         simtime_t emitTime_st = simTime()+1;
         double emitPosition = 0.0;
         double emitSpeed = 0;
         int8_t emitLane = 0;
   //  std::cout << "Adding new vehicle since this one is leaving\n";
    // std::cout << "New vehicle being added to route " << routeId << " with id: " << vehicleId <<
    //         " with type id " << vehicleTypeId << "and route id " << routeId << " and emit time " << emitTime_st <<
      //       std::endl;
         traci->addVehicle(vehicleId, vehicleTypeId,
                   routeId, emitTime_st, emitPosition, emitSpeed, emitLane);
         vehicleSent = true;
     }else{ // junction is busy but there is no queue anymore; vaporize the vehicle at this position if it is virtual
         removeVirtualCar();
     }
 }else{
     removeVirtualCar();
 }
*/
// EAST WEST
/**  if(traci->getVehicleIds().size() <= N && curDirection.x == -1.0 && traciVehicle->getLaneId() == "H4G4_0" &&
    traciVehicle->getLanePosition() >= 880.0 && traciVehicle->getLanePosition() < 890.0){
// make this car wait at intersection if there is other car within 7 meters of start of road
if(checkIfQueue("H4G4_0")){
//    std::cout << "Setting max speed of car: " << traciVehicle->getVehicleId() << std::endl;
 //   traciVehicle->setMaxSpeed(0.0);

  //  traciVehicle->stopAt(traciVehicle->getRoadId(), 890.0, 0, 0.0, 1.0);
//  lastAccWaitTime+=1.0;
    //traciVehicle->stopAt(traciVehicle->getRoadId(), 890.0, 0, 0.0, 1.0);
    if(traciVehicle->getSpeed() <= 3.0) traciVehicle->slowDown(0.0, 1.0);
/*    std::pair<std::string, double> leaderPair = traciVehicle->getLeader(5.0);
    std::cout << "This vehicle " << traciVehicle->getVehicleId() << " has a leader with id "
            << leaderPair.first << " that is " << leaderPair.second << " away from this car \n " << std::endl;
    if((std::string)leaderPair.first == ""){
        std::cout << "This vehicle has no leading vehicle \n " << std::endl;
        traciVehicle->stopAt(traciVehicle->getRoadId(), 890.0, 0, 0.0, 1.0);
    }*/

/**   }
//traciVehicle->stopAt(traciVehicle->getRoadId(), 890.0, 0, 0.0, 1.0); // wait until next time step?
// NORTH SOUTH
}else if(curDirection.y == 1.0  && traciVehicle->getLaneId() == "G5G4_0" &&
    traciVehicle->getLanePosition() < 880.0 && traciVehicle->getLanePosition() > 870.0){
// make this car wait at intersection if there is other car within 7 meters of start of road
if(checkIfQueue("G5G4_0") && (traciVehicle->getSpeed()) > 2.0){
 //   std::cout << "Setting max speed of car: " << traciVehicle->getVehicleId() << std::endl;
  //  traciVehicle->setMaxSpeed(0.0);
 //   traciVehicle->stopAt(traciVehicle->getRoadId(), 890.0, 0, 0.0, 1.0);
  // lastAccWaitTime += 1.0;
}
// traciVehicle->stopAt(traciVehicle->getRoadId(), 890.0, 0, 0.0, 1.0);
}else{
// go ahead
}
//}
*/

