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

#include "veins/modules/application/traci/ASLAppRSU.h"

using namespace veins;

Define_Module(veins::ASLAppRSU);

void ASLAppRSU::initialize(int stage)
{
    DemoBaseApplLayer::initialize(stage);
    if (stage == 0) {
        // Initializing members and pointers of your application goes here
        EV << "Initializing " << par("appName").stringValue() << std::endl;
        EV << "Simulating for " << N << " vehicles \n";
        // initialize pointers to other modules
        if (FindModule<TraCITrafficLightInterfaceAccess*>::findSubModule(getParentModule())) {
            tlsInterface = TraCITrafficLightInterfaceAccess().get(getParentModule());
            tlsInterface->setExternalId("G4");
            traci = tlsInterface->getCommandInterface();
            tlsTraci = tlsInterface->getTlCommandInterface();
        }
        else {
            //traci = nullptr;
        }
    }
    else if (stage == 1) {
        // Initializing members that require initialized other modules goes here
    }

    //tlsInterface = new TraCITrafficLightInterface();
    //tlsInterface->preInitialize();

}

void ASLAppRSU::initialize(int stage, int deviceType){
    // deviceType = 0 -> vehicle
    // deviceType = 1 -> RSU
    initialize(stage);
    this->deviceType = deviceType;
    /////// TODO: MAY NEED TO MAKE HASHMAP THAT CARS USE FOR FIRST TIME TO
    if(!gpsAttackOn && !speedAttackOn && !signalAttackOn && !loopAttackOn){
        std::cout << "There Are No Attacks\n";
    }else{
        filename.append("Attack_");
    }
    if(gpsAttackOn){
        std::cout << "Position Spoofing (GPS/V2X) Attack is On\n";
        filename.append("gps_");
    }
    if(speedAttackOn){
        std::cout << "Speed Spoofing (V2X) Attack is On\n";
        filename.append("speed_");
    }
    if(signalAttackOn){
        std::cout << "Signal Jamming Attack (V2X) Attack is On\n";
        filename.append("tcomm_");
    }
    if(loopAttackOn){
        std::cout << "Induction Loop Attack is On\n";
        filename.append("loop_");
    }

    std::ofstream outputFile (headway_output);
    outputFile.close();
    sleep(2000);
}

void ASLAppRSU::finish()
{

//    if(onASLBool){
//        filename.append("WaitLogic_ASL_PerformanceResults_N");
//    }else{
//        filename.append("WaitLogic_NoASL_PerformanceResults_N");
//    }
//    filename.append(std::to_string((int)(N)));
//    filename.append(".txt");
////    std::ofstream outputFile(filename);
////    outputFile <<  "\n Number of vehicles is: " << N << std::endl;
//    // figure out how to get overall traffic flow here and print it out or log it
//    std::list<std::string> roadIds = traci->getRoadIds();
//    std::string currRoadId;
//    int totalRoads = roadIds.size();
//    TraCICommandInterface::Road* currRoad;
    //while(!roadIds.empty()){
    //    currRoadId = roadIds.front();

//    // vertical road avg speed
//    currRoadId = "G5G4"; // vertical
//    currRoad = new TraCICommandInterface::Road(traci, currRoadId);
//    double tempAvgSpeed = currRoad->getMeanSpeed();
//    globalMeanSpeedNS += tempAvgSpeed;
// //   outputFile << "Road " << currRoadId << " has last step mean speed " << currRoad->getMeanSpeed() << std::endl;
//    // horizontal road avg speed
//    currRoadId = "H4G4"; // horizontal
//    currRoad = new TraCICommandInterface::Road(traci, currRoadId);
//    tempAvgSpeed = currRoad->getMeanSpeed();
//    globalMeanSpeedEW += tempAvgSpeed;
// //   outputFile << "Road " << currRoadId << " has last step mean speed " << currRoad->getMeanSpeed() << std::endl;
//
//    totalStepsEW++;
//    totalStepsNS++;
//    //globalMeanSpeed /= (double)totalRoads; // total global speed across all roads for whole time
//    globalMeanSpeedNS /= totalStepsNS; // total time of simulation in seconds
//    globalMeanSpeedEW /= totalStepsEW;
    //globalMeanSpeedEW /= 2.0;
//    outputFile << "\nNS Final Global Mean Speed is: " << globalMeanSpeedNS << std::endl;
//    outputFile << "EW Final Global Mean Speed is: " << globalMeanSpeedEW << std::endl;
//    outputFile << "EW total steps " << totalStepsEW << std::endl;
//    outputFile << "NS Alternate Junction Avg Flow Rate is: " << altAvgFlowRateNS/totalStepsNS << std::endl;
//    outputFile << "EW Alternate Junction Avg Flow Rate is: " << altAvgFlowRateEW/totalStepsEW << std::endl;
//    outputFile << "NS total waiting time (s) is: " << (totalWaitingTimeNS/100)  << std::endl;
//    outputFile << "EW total waiting time (s) is: " << (totalWaitingTimeEW/100)  << std::endl;
//   // outputFile << "Last time in RSU is: " << time << std::endl;
//   // outputFile << "Total simulation time is: " << simTime().dbl() << std::endl;
//    outputFile << "ACTUAL total simulation time is: " << simTime().dbl() - initTime << std::endl;
//    // can have a total time for EW and for NS by saving it when there are no cars in the corresponding lane
//    outputFile << "\nnumvehiclespassed1ns: " << totalCarsPassed1_NS << std::endl;
//    outputFile << "numvehiclespassed2ns: " << totalCarsPassed2_NS << std::endl;
//    outputFile << "numvehiclespassed1ew: " << totalCarsPassed1_EW << std::endl;
//    outputFile << "numvehiclespassed2ew: " << totalCarsPassed2_EW << std::endl;
//    outputFile << "Total cars that exited NS is: " << totalExitedCarsNS << std::endl;
//    outputFile << "Total cars that exited EW is: " << totalExitedCarsEW << std::endl;
//    outputFile << "Estimated EW waiting time ratio is: " << ((totalWaitingTimeEW/100) / (totalStepsEW)  ) << std::endl;
//    outputFile.close();

    DemoBaseApplLayer::finish();
//    // statistics recording goes here
//    recordScalar("RSUgeneratedASLMsgs", generatedASLs);
//    recordScalar("RSUreceivedASLBSMs", receivedASLBSM);
    delete(tlsInterface);
}

void ASLAppRSU::onBSM(DemoSafetyMessage* bsm)
{

}

void ASLAppRSU::onWSM(BaseFrame1609_4* wsm)
{
    // Your application has received a data message from another car or RSU
    // code for handling the message goes here, see TraciDemo11p.cc for examples
}

void ASLAppRSU::onWSA(DemoServiceAdvertisment* wsa)
{
    // Your application has received a service advertisement from another car or RSU
    // code for handling the message goes here, see TraciDemo11p.cc for examples

}

void ASLAppRSU::onASL(ASLMessage* asl)
{
    // Your application has received an ASL packet from an RSU
    // code for handling the message goes here, see TraciDemo11p.cc for examples
}

/*
// Your application has received an ASL packet from an RSU
// code for handling the message goes here, see TraciDemo11p.cc for examples
// Your application has received a beacon message from another car or RSU
// code for handling the message goes here*/
void ASLAppRSU::onASLBSM(ASLBSM* aslBsm)
{
    simtime_t currTime = simTime();
    tlsInterface->setExternalId("G4");
    traci = tlsInterface->getCommandInterface();
    tlsTraci = tlsInterface->getTlCommandInterface();
    int currPhaseIdx;
    simtime_t nextSwitchTime;
    LAddress::L2Type senderId = aslBsm->getSenderAddress();
    Coord position = aslBsm->getSenderPos();
    Coord speed = aslBsm->getSenderSpeed();
    Coord direction = aslBsm->getDirection();
    Heading heading = aslBsm->getHeading();
    std::string currVehicleId = aslBsm->getVehId();
    TraCICommandInterface::Vehicle* veh = new TraCICommandInterface::Vehicle(traci, currVehicleId);
    //double XYZPosition = convertLonLat2XY(position);
    double curPositionX = position.x; // POSITION ATTACK HERE E.G. GPS OR FAKE MSG
    double curPositionY = position.y;
    double currSpeed = speed.length();
    double aslSpeed = currSpeed;
    std::time_t t;
    /* Intializes random number generator */
    std::srand((unsigned) (std::time) (&t));
    attackRandSeed = 0; // so that we get different values from uniform distribution each simulation

  //  std::cout<< "RSU:: This vehicle " << currVehicleId << " currently is going at speed " << speed << " and at direction " << direction << " and at pos " << position << std::endl;
    // get speed in the X direction?
   // std::cout << "This vehicle is going at speed: " << currSpeed << std::endl;

    // time at which the packet arrives to RSU is different than when vehicle sent the packet
    // so need to adjust accordingly to possible delay
    time = currTime.dbl();

    // if the  car is not able to make the current red light then it needs to be put into the next green cycle
    // if the car is within the current green cycle, then it will be given a max asl speed based on the current green cycle and its current speed + current time
    if(logging) std::cout << "RSU:: The time that I computed is: " << time << std::endl;
    if(logging) std::cout << "RSU receiving packet from vehicle " << currVehicleId << " at time " << simTime().dbl() << std::endl;

    ///////TEST: speedAttack()
    if(speedAttackOn){
        if(isRandomTimeAttack){ ///if there is a random chance that this attack will occur at this time t, for this car
            if(uniform(0.0, 1.0, attackRandSeed) <= randomTimeFreq){ //if rand val > given threshold
                currSpeed = speedAttack(currSpeed, expArrivalTimes, senderId);
            }
        }else{ // else, just perform attack
            currSpeed = speedAttack(currSpeed, expArrivalTimes, senderId);
        }
    }


    // VEHICLESINFRONT - INDUCTION LOOP ATTACK HERE
    // Need intersection position and need range of positions for oncoming vehicles:
    // if coming from the right:
    double val1;
    double distToInter;
    double carsInFront;
     double gps_err = uniform(0.01, 5.00); // random gps error in range of [-3,3]
     if(std::rand()%2==1){
         gps_err = -gps_err; // 50% chance we make this into a negative
     }
     ////std::cout << "Gps error is: " << gps_err << std::endl;
     double tcomm_delay = 1.5;

     // compute the estimated actual lane position that this vehicle is in
     double adjustedLanePos;
     adjustedLanePos = veh->getLanePosition() + gps_err + (std::abs(currSpeed) * tcomm_delay);

     ///////TEST: positionAttack()
     // gps attack
     if(gpsAttackOn){
      //   std::cout << "Original adjusted lane for vehicle " << currVehicleId << " is " << adjustedLanePos << std::endl;
         if(isRandomTimeAttack){ ///if there is a random chance that this attack will occur at this time t, for this car
             if(uniform(0.0, 1.0, attackRandSeed) <= randomTimeFreq){
                 std::srand((unsigned) (std::time) (&t));
                 if(uniform(0.0, 1.0, attackRandSeed) <= randomCarFreq){ //if rand val > given threshold
                     adjustedLanePos = positionAttack(veh->getLanePosition() + gps_err +
                         (std::abs(currSpeed) * tcomm_delay),
                         expArrivalTimes, senderId);
                 }
             }
         }else if(isPeriodicAttack){
             // attack will only occur during specified duration/period; each attack cycle length
             // is considered as attackperiod + attackoffset
             if (std::fmod(time, attackPeriod + attackOffset) <= attackPeriod){
                 // if the time is within the green time, we want to make the alpha negative; else if in the red time we want to make it pos
                 adjustedLanePos = positionAttack(veh->getLanePosition() +
                         gps_err +  (std::abs(currSpeed) * tcomm_delay),
                             expArrivalTimes, senderId);
             }
         }
         else{ // else, just perform attack
             // if the time is within the green time, we want to make the alpha negative; else if in the red time we want to make it pos
             adjustedLanePos = positionAttack(veh->getLanePosition() + gps_err +
                     (std::abs(currSpeed) * tcomm_delay),
                         expArrivalTimes, senderId);
         }
      ///////   std::cout << "Original adj. lane pos for veh "<<currVehicleId << " is " << veh->getLanePosition() + gps_err +  (std::abs(currSpeed) * tcomm_delay) << std::endl;
     }
     adjustedLanePos = std::max(adjustedLanePos, 600.0);
     adjustedLanePos = std::min(adjustedLanePos, 889.9);
     /////////std::cout << "Attacked adj. lane pos for veh " << currVehicleId << " is " << adjustedLanePos << std::endl;
     distToInter = 890.0-adjustedLanePos;

     double arrivalTime = 0.0;
     if(expArrivalTimes->end() == expArrivalTimes->find(senderId)){
        // first value is based on the current position of the vehicle and the distance between it and the intersection
        // assuming the car can go at free flow speed, what is the expected arrival time
        // coming from east
        if(direction.x == -1.0){
            //val1 = time + (L-adjustedLanePos-gps_err -(std::abs(currSpeed) * tcomm_delay))/speedLimit;
            val1 = time + (890.0-adjustedLanePos)/speedLimit;
            carsInFront = (double) carsInFront_EW; // carsInFront_EW is updated in periodic RSU environment check
            //if(adjustedLanePos > 880.0)
           if(logging){
                std::cout << "RSU: Time: " << currTime.dbl() << std::endl;
                std::cout << "RSU: This car coming from east at position " << position << std::endl;
                std::cout << "RSU: Cars in front EW (estimated): " << carsInFront_EW << std::endl;
           }
        }
        // if this car is in first 300 meters at the start, then they have a unique cars_in_front value that we must use instead
        if(direction.x == -1.0 &&
            specialCarsEW->end() != specialCarsEW->find(currVehicleId)){
            carsInFront = 0.0;
        }

   ////     std::cout<< "Cars in front value for car " << currVehicleId << " is: " << carsInFront << std::endl;
        carsInFront = std::max(carsInFront, 0.0);
   /////     std::cout<< "Cars in front value for car " << currVehicleId << " is: " << carsInFront << std::endl;

        double val2 = time + (carsInFront * serviceRate); //  tau aka mingap * service rate (seconds per vehicle) aka 3600/1800
       if(logging){ std::cout << "Computed arrival time 1: "<< val1 << std::endl;
        std::cout << "Computed arrival time 2:  " << val2 << std::endl;
       }
        // Expected arrival time based on the cars in front of the current vehicle
        arrivalTime = std::max(val1, val2);
    }

    // if this car already has an entry of its original expected arrival time stored,
    // we dont need to calculate a new one based on cars in front.
    //just update the asl speed instead based on old one (unless old one is bad)
   if(expArrivalTimes->end() != expArrivalTimes->find(senderId)){
        //std::cout << "Vehicle " << currVehicleId << " already has expected arrival time: " << expArrivalTimes->at(senderId) << std::endl;
        arrivalTime = expArrivalTimes->at(senderId); // get the last expected arrival time value
        if(arrivalTime <= time){ // this car was supposed to arrive earlier so we have to update this arrival time now
            if(logging) std::cout << "We need to update this vehicle's arrival time since it cannot arrive by expected time anymore " << std::endl;
            arrivalTime = time + (distToInter)/(speedLimit);
            // update the contents
            (*expArrivalTimes)[senderId] = arrivalTime;
        }
    }

    // make sure the car is arriving at valid time and update v_asl accordingly
   // for phase 1 cars: [0-26] that arrive within [31-57] of cycle - EW cars
   if(logging) std::cout << "Arrival time is: " << arrivalTime <<  " direction is " << direction << " and arrival time mod is: " << std::fmod(arrivalTime, T) << std::endl;
   if(std::fmod(arrivalTime, T) >=30.0 && direction.x == -1.0){
       //if(logging)  std::cout << "RSU: EW car arriving in red time " << arrivalTime << ", so we change to next green time: ";
        arrivalTime = arrivalTime + T - std::fmod(arrivalTime, T);
        (*expArrivalTimes)[senderId] = arrivalTime;
      if(logging)  std::cout << "" << arrivalTime << std::endl;
    }
    if(expArrivalTimes->end() == expArrivalTimes->find(senderId)){
        // if there is no entry, we need to add the first expected arrival time into the map
        (*expArrivalTimes)[senderId] = arrivalTime;
    }
   double aslSpeed_est = (distToInter) / (arrivalTime-time);
   aslSpeed = std::min(speedLimit, aslSpeed_est);

   if(logging){
        std::cout << "RSU:: For vehicle " << senderId << ", the car's distance to intersection is: " << distToInter << std::endl;
        std::cout << "RSU:: The total additional position adjustment is: " << (gps_err + (currSpeed * tcomm_delay)) << std::endl;
        std::cout << "RSU: arrival time is: " << arrivalTime << " and current time is: " << time << std::endl;
        std::cout << "RSU: The estimated asl speed is: " << aslSpeed_est << " and chosen asl speed is: " << aslSpeed << std::endl;
   }

    ASLMessage* asl = new ASLMessage();
    LAddress::L2Type myId = mac->getMACAddress();
    asl->setAslSpeed(aslSpeed); // get the average speed (sqrt (x^2 + y^2 + z^2)) // SPEED ATTACK HERE
    asl->setArrivalTime(arrivalTime); // ARRIVAL TIME ATTACK HERE
    ASLAppRSU::populateASL(asl, myId, senderId);
    sendDelayedDown(asl, 1.5); // send the packet
}

void ASLAppRSU::populateASL(ASLMessage* asl, LAddress::L2Type senderId, LAddress::L2Type rcvId){
    asl->setRecipientAddress(rcvId);
    asl->setBitLength(headerLength);
    asl->setPsid(-1);
    asl->setChannelNumber(static_cast<int>(Channel::cch));
    asl->addBitLength(beaconLengthBits);
    asl->setUserPriority(beaconUserPriority);
}

void ASLAppRSU::populateWSM(BaseFrame1609_4* wsm, LAddress::L2Type senderId, LAddress::L2Type rcvId){
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

void ASLAppRSU::handleLowerMsg(cMessage* msg){
    BaseFrame1609_4* wsm = dynamic_cast<BaseFrame1609_4*>(msg);
    ASSERT(wsm);

    // asl message is a unique message for this application so we do not include it in the base demo app layer
    if (ASLMessage* asl = dynamic_cast<ASLMessage*>(wsm)) {
        receivedASL++;
      // delete(msg);
      // onASL(asl);
      // ignore asl messages if RSU
    }else if(ASLBSM* aslBsm = dynamic_cast<ASLBSM*>(wsm)){
        receivedASLBSM++;
        onASLBSM(aslBsm);
      //  delete(msg);
    }else{
     //   DemoBaseApplLayer::handleLowerMsg(msg);
    }
    delete(msg);

}

void ASLAppRSU::handleSelfMsg(cMessage* msg)
{
    simtime_t currTime = simTime();
    tlsInterface->setExternalId("G4");
    traci = tlsInterface->getCommandInterface();
    tlsTraci = tlsInterface->getTlCommandInterface();
    int currPhaseIdx;
    simtime_t nextSwitchTime;
   // DemoBaseApplLayer::handleSelfMsg(msg);
    // this method is for self messages (mostly timers)
    // it is important to call the DemoBaseApplLayer function for BSM and WSM transmission
    if(tlsInterface){
        std::list<std::string> roadIds = traci->getRoadIds();
        std::string currRoadId;
        std::string currLaneId;
        int totalRoads = roadIds.size();
        TraCICommandInterface::Road* currRoad;
        double tempAvgSpeedNS;
        double tempAvgSpeedEW;
        //std::list<std::string> vehicleIds = traci->getVehicleIds();
        int currEWCars = 0;
        int currNSCars = 0;
        double currWaitTimeEW = 0.0;
        double currWaitTimeNS = 0.0;
        TraCICommandInterface::Lane* currLane;
        std::list<std::string> junctionIds = traci->getJunctionIds();
        if(!junctionIds.empty()){
            Coord junctionPos = traci->junction(junctionIds.front()).getPosition();
            // can check how many cars were before this junction and after for a given time
        }

        if(simTime().dbl() > 2.5 && !carsPassedInit){
            updateEWSpecialCarCarsInFront();
            carsPassedInit = true;
        }

        std::list<std::string> laneAreaDetectors = traci->getLaneAreaDetectorIds();
        std::list<std::string> lanes = traci->getLaneIds();
        if(!laneAreaDetectors.empty() ){
            //// EAST WEST DIRECTION CARS IN FRONT OF INDUCTION LOOP
            int numVehiclesPassed1 =
                    traci->laneAreaDetector("e2Detector_H4G4_0_0").getLastStepVehicleNumber();
            std::list<std::string> vIds =
                    traci->laneAreaDetector("e2Detector_H4G4_0_0").getLastStepVehicleIds();
            if(!vIds.empty())
            {
              //  std::cout << "RSU: number of detected cars on EW sensor1 is: "<< vIds.size() << std::endl;
                if(vIds.front()!=lastvehIdEW1){
                    // this is an unseen car (not waiting at loop detector). must add/subtract from total detections
                    totalCarsPassed1_EW += numVehiclesPassed1;
                    //std::cout << "numvehiclespassed1ew: " << totalCarsPassed1_EW << std::endl;
                    lastvehIdEW1=vIds.front();
                }
            }

            int numVehiclesPassed2 = traci->laneAreaDetector("e2Detector_H4G4_0_1").getLastStepVehicleNumber();
            vIds = traci->laneAreaDetector("e2Detector_H4G4_0_1").getLastStepVehicleIds();
            if(!vIds.empty())
            {
                if(vIds.front()!=lastvehIdEW2){
                    // this is an unseen car (not waiting at loop detector). must add/subtract from total detections
                    totalCarsPassed2_EW += numVehiclesPassed2;
                  //  std::cout << "numvehiclespassed2ew: " << totalCarsPassed2_EW << std::endl;
                    lastvehIdEW2=vIds.front();
                }
            }
            //std::cout << "Total cars in application area is: " << totalCarsPassed1_EW-totalCarsPassed2_EW << std::endl;
            carsInFront_EW = std::max(0,totalCarsPassed1_EW-totalCarsPassed2_EW);
            if(totalCarsPassed2_EW>totalCarsPassed1_EW) totalCarsPassed2_EW = totalCarsPassed1_EW; // even them out

            ///////ATTACK TEST: inductionLoopAttack() on EW/NS induction loop
            if(loopAttackOn){
                LAddress::L2Type senderId = 0;
                if(isRandomTimeAttack){ ///if there is a random chance that this attack will occur at this time t, for this car
                    if(uniform(0.0, 1.0, attackRandSeed) >= randomTimeFreq){ //if rand val > given threshold
                         carsInFront_EW = inductionLoopAttack(carsInFront_EW, expArrivalTimes, senderId);
                    }
                }else{ // else, just perform attack
                    carsInFront_EW = inductionLoopAttack(carsInFront_EW, expArrivalTimes, senderId);
                }
            }

            carsInFront_EW = std::min(41, carsInFront_EW); // 42 is max in 300 m
            carsInFront_EW = std::max(0, carsInFront_EW);
        }
    }
    //std::cout << "The beacon interval in seconds is: " << beaconInterval.dbl() << std::endl;
    scheduleAt(simTime() + beaconInterval.dbl(), sendBeaconEvt);
}

void ASLAppRSU::updateEWSpecialCarCarsInFront(){
    // EW lane cars
    std::string currRoadId = "H4G4"; // horizontal
    std::string currLaneId = "H4G4_0";
    TraCICommandInterface::Lane* EWLane = new TraCICommandInterface::Lane(traci, currLaneId);
    std::list<std::string> ewcars = traci->getVehicleIds();
    TraCICommandInterface::Vehicle* ewcar;
    double ewcarpos;
    while(!ewcars.empty()){
        ewcar = new TraCICommandInterface::Vehicle(traci, ewcars.front());
        ewcarpos = ewcar->getLanePosition();
       // std::cout << "Vehicle " << ewcars.front() << " has lane position " << ewcarpos << std::endl;
        if(ewcarpos >= 600.0) // this car is within detector range
        {
            specialCarsEW->insert(std::pair<std::string, int>(ewcars.front(), (int)(L-ewcarpos)/sumoDepartDist));
            totalCarsPassed1_EW++;
           // std::cout<< "Updating cars in front value for EW car " << ewcars.front() << " to: " << (int)((L-ewcarpos)/(sumoDepartDist)) << std::endl;
        }
        ewcars.pop_front();
    }
   // std::cout << "total cars passed 1 after update special ew fn is: " << totalCarsPassed1_EW << std::endl;
}

void ASLAppRSU::updateNSSpecialCarCarsInFront()
{
  //  std::list<std::string> vehicleIds = traci->getVehicleIds();
    // NS lane cars
    std::string currRoadId = "G5G4"; // vertical
    std::string currLaneId = "G5G4_0";
    TraCICommandInterface::Lane* NSLane = new TraCICommandInterface::Lane(traci, currLaneId);
    std::list<std::string> nscars = NSLane->getLastStepVehicleIds();
    TraCICommandInterface::Vehicle* nscar;
    double nscarpos;
    while(!nscars.empty()){
        nscar = new TraCICommandInterface::Vehicle(traci, nscars.front());
        nscarpos = nscar->getLanePosition();
        std::cout << "This car " << nscars.front() << " has position " << nscarpos << std::endl;
        if(nscarpos >= 600.0) // this car is within detector range
        {
            // if this car is the foremost leading car (cars in front value becomes 0) then
            specialCarsNS->insert(std::pair<std::string, int>(nscars.front(), (int)(L-nscarpos)/sumoDepartDist));
            std::cout<< "Updating cars in front value for NS car " << nscars.front() << " to: " << (int)((L-nscarpos)/(sumoDepartDist)) << std::endl;
        }
        nscars.pop_front();
    }
}

void ASLAppRSU::handlePositionUpdate(cObject* obj)
{
   // DemoBaseApplLayer::handlePositionUpdate(obj);
    // the vehicle has moved. Code that reacts to new positions goes here.
    // member variables such as currentPosition and currentSpeed are updated in the parent class
}
void ASLAppRSU::checkAndTrackPacket(cMessage* msg)
{
    if(dynamic_cast<ASLMessage*>(msg)){
        EV_TRACE << "sending down an ASL" << std::endl;
        generatedASLs++;
    }else {
        DemoBaseApplLayer::checkAndTrackPacket(msg);
    }
}
