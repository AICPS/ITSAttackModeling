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

#include <algorithm>
#include <iostream>
#include <fstream>
#include "veins/veins.h"
#include "veins/modules/application/ieee80211p/DemoBaseApplLayer.h"
#include "veins/modules/messages/ASLMessage_m.h"
#include "veins/modules/application/traci/ASLBSM_m.h"
#include "veins/modules/world/traci/trafficLight/TraCITrafficLightInterface.h"
#include "veins/modules/mobility/traci/TraCICommandInterface.h"
#include "veins/modules/application/traci/RSUAttack.h"
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

class VEINS_API ASLAppRSU : public DemoBaseApplLayer {
public:
    void initialize(int stage) override;
    void initialize(int stage, int deviceType);
    void finish() override;



protected:
    double MPR = 1.0; // market penetration rate; used to determine if the current car is / isn't connected
    std::list<bool>* isConnectedList = new std::list<bool>(); // stores the conenctivity status of each car
    bool logging = false; // boolean to turn on/off debugging logging
    double speedLimit = 15.0; // 15 m/s -> make sure these are consistent with sumo roads ; aka free flow speed
    double L = 890.0; // road length
    double L_int = 10.0;
    double T = 60.0;
    double T_green = 30.0;
    double T_yellow = 30.0-T_green;
    double time = 0.0;
    double initTime = 10.0 * T;
    double serviceRate = 2.5; // 2, 2.5
    double jam_dist = 7.0; // includes road distance and headway (2 meters)
    double N=50; // number of vehicles
    double sumoDepartDist = jam_dist; // must change this according to the number of vehicles; this is space per vehicle + min distance/car
    bool onASLBool=true;
    double comm_delay=1.5;

    /* ATTACK MODELS */
    /*
     * Value Tampering:
     * Random/Deterministic
     * Stealthy, Medium, Heavy
     * Range of Values
     *
     * Timing:
     * Random/Deterministic Frequency
     * Duration (t_start, t_end)
     *
     */
/*  See RSUAttack.h for details
 * bool commAttackOn = false;
    bool gpsAttackOn = true;
    bool speedAttackOn = false;
    bool signalAttackOn = false;
    bool loopAttackOn = false;
    bool isRandomTimeAttack = false; // flag to determine if the moment of attack is chosen at random each time, or static / deterministic
    bool isRandomValAttack = false; // flag to determine if the value of attack is chosen at random or not
    double randomTimeFreq = 0.0; // how frequently the random value is changed; two dimensions: time and cars
    double randomCarFreq = 0.0; // which cars are attacked with this model
    double attackStartTime = 0.0; // start time of the attack
    double attackEndTime = 0.0; // end time of the attack -> endtime - starttime = duration
    double attackFrequency = 0.0;
    int impactLevel = 0; // 0, 1, 2 -> stealthy, medium, heavy ; correspond to different levels of impact based on the targeted variable
    double minPerturb = 0.0;
    double maxPerturb = 1.0; // this is the range of perturbation that is permitted and based on these and the impact level, the final value is selected
*/
    // this and the timing depend on the attacker's resources
    // each function will have a range of permissible values as well so that the attacked value does not go overboard

    double randomSelect(double first, double second){
        // can choose what kind of random distribution  here as well. we make all values fair for now
        return uniform(first, second, attackRandSeed);
    }

    /* Targets the gps sensor on a vehicle with an imagined spoofing attack; aka a position spoofing attack
     * This can also be done via communication-based attacks (hacked car/obu sends faked position values).
     * Lane position value (not real position, for the sake of simplicity)
     */
    double positionAttack(double origVal, std::map<long, double>* expArrivalTimes, LAddress::L2Type senderId){
        double attackVal = 0.0;
        if(impactLevel==0){
            minPerturb = 0.0;
            maxPerturb = 5.0;
        }else if(impactLevel==1){
            minPerturb = 0.0;
            maxPerturb = 60.0;
        }else{
            minPerturb = 150.0;
            maxPerturb = 299.0; //really high delay
        }

        // make the perturbation positive but change the sign according to the situation (if light is green or red)
        // the attack is random / not random based on range of values
        if(isRandomValAttack){ // random
            double perturb = randomSelect(minPerturb, maxPerturb);
            attackVal = origVal + perturb;
        }else{ // static
            attackVal = origVal + staticXAlpha;
        }
        return attackVal;
    };
    /* Targets the speed value sent in a message via communication-based attacks (hacked car/obu sends faked speed values) */
    /*
     *  Speed rang is from 0-15 so we have to make sure this value does not go beyond that.
     */
    double speedAttack(double origVal, std::map<long, double>* expArrivalTimes, LAddress::L2Type senderId){
        double attackVal = 0.0;
        if(impactLevel==0){
            minPerturb = 1.0;
            maxPerturb = 3.0;
        }else if(impactLevel==1){
            minPerturb = 3.0;
            maxPerturb = 7.5;
        }else{
            minPerturb = -7.5;
            maxPerturb = -15.0; //really high delay
        }


        if(isRandomValAttack){ // random
            double perturb = randomSelect(minPerturb, maxPerturb);
            attackVal = origVal + perturb;
        }else{ // static
            attackVal = origVal + staticVAlpha;
        }
        attackVal = std::min(attackVal, 15.0);
        attackVal = std::max(attackVal, 0.0);
        return attackVal;
    };
    /* Targets the traffic controller cabinet/ RSU that handles the traffic signal timing plan. May delay or speed up timing to cause shockwaves.
     * Even small shockwaves can be created that can build up to be a big one later. Could be related to the harmonic oscillation of the waves.
     * */
    void trafficSignalAttack(double origVal, std::map<long, double>* expArrivalTimes, LAddress::L2Type senderId){
        // TODO
    };
    /* Targets the number of detected vehicles by the induction loop sensor (positioned at 600meters)*/
    int inductionLoopAttack(int origVal, std::map<long, double>* expArrivalTimes, LAddress::L2Type senderId){
        int attackVal = 0.0;
        // The following is ignored for static attacks
        if(impactLevel==0){
            minPerturb = 1;
            maxPerturb = 10;
        }else if(impactLevel==1){
            minPerturb = 10;
            maxPerturb = 20;
        }else{
            minPerturb = 20;
            maxPerturb = 41; //really high delay
        }

//            if(expArrivalTimes->find(senderId)!=expArrivalTimes->end()){
//
//                // if the time is in green time, we want the car to appear closer so that their velocity is less ?
//                // if the time is in red time, we want the car to appear farther so that their velocity is more ?
//                if(fmod((*expArrivalTimes)[senderId], T)<=T_green){
//                    minPerturb = 0-minPerturb;
//                    maxPerturb = 0-maxPerturb;
//                }
//            }

        if(isRandomValAttack){
            double perturb = randomSelect(minPerturb, maxPerturb);
            attackVal = origVal + (int)perturb;
        }else{
            attackVal = origVal + staticVIFAlpha;
        }
        return attackVal;
    };


    double worstCaseAttack_Position(){ // TODO

        // given x, and v, and ti, we determine what the best spoof on position we can do to
        // make the current vehicle arrive at the red time

        // make the vehicle arrive at red time, or at time after the car in front of it during red time

         /*
          * % the worst-case aka intelligent attacker will use the targeted variable(s)
% to make sure that the vehicle always arrives on the red time (unless
% the vehicle is too close to the intersection)
% It is important that the attacker uses either x, tcomm, or vel_asl
% to perform the attack since at every time step after the first time it receives asl
% the vehicle will receive a new valid one and will reduce the impact of
% the attack.
function [hat_v_asl, x_alpha] = ...
getWorstCaseAttackerASL(cnt,currtime,attack_flags,...
attack_vector,x,cars_in_front,ti,vf,v_asl_old)

% attack flags can be used to identify which variables are targeted to
% perform the attack

%% get the parameters
att_start=attack_vector(1);
att_duration=attack_vector(2);
time_att_ratio=attack_vector(3);
isPeriodic=attack_vector(4);
att_period=attack_vector(5);
att_offset=attack_vector(6);
veh_att_ratio=attack_vector(7);

hat_v_asl = 0.0;
v_delt = 0;
x_delt = 0;
tcomm_delt = 0;
v_delt = 0;
L1 = 890;
t_delta = 1.5;
R = 30.0/(t_delta);
T = 60.0/(t_delta);
g_time = 24.0;
y_time = 6.0;
T_time = 60.0;
x_err_est = -5 + (5+5)*rand(1);
sr = 1800/3600;
curr_step = mod((cnt-1),T);
mod_time = mod(currtime,T_time);

% need to predict the new ti if t > ti
if(currtime > ti)
    ti = currtime + (890-(x+x_err_est))/vf;
    if(mod(ti,T_time) >= g_time+y_time) % need to verify if this is correct logic
        ti = ti+T_time-mod(ti,T_time);
    end
end

%
%     if(ti >= currtime + (T_time-mod(currtime, T_time)))
%         hat_v_asl = vf;
%         x_alpha = 0;
%         return;
%     end
%     %
%
dist_to_inter = L1 - (x+x_err_est);

if(mod(currtime,T_time) >= g_time + y_time)
   hat_v_asl = vf;
   x_alpha = -(300-dist_to_inter);
   return;
end

%% Attack
% goal is: v_asl * (R-t) = 900-x = dist to inter
g_t_left = 0;
if(mod_time <= 29.0)
    g_t_left = (29.0-mod_time);
else
    g_t_left = (T_time-mod_time)+29.0;
end

%     if(g_t_left * vf < L1 - (x+x_err_est))
%         hat_v_asl = vf;
%         x_alpha = 0;
%         return;
%     end

hat_v_asl = g_t_left/dist_to_inter;
hat_v_asl = min(hat_v_asl, vf);

%     x_alpha = dist_to_inter-(hat_v_asl * ...
%         (g_t_left));
x_alpha = dist_to_inter * (1-((ti - currtime)/g_t_left));
% hat_x_alpha = (L1-x)(1-(ti-t)/(tr-t))
          *
          *
          * */

        return 0.0;
    }

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
    void updateEWSpecialCarCarsInFront();
    void updateNSSpecialCarCarsInFront();




    std::string headway_output = "headwayResults.txt";
    bool startPrinted = false;
    // the following is important if we are setting departPos = "last" or if there are vehicles already past the induction loop at insertion
    //int totalCarsPassed1_NS= (int)(300.0/(sumoDepartDist));
    int totalCarsPassed1_NS=0;
    int totalCarsPassed2_NS=0;
    //int totalCarsPassed1_EW=(int)(300.0/(sumoDepartDist));
    int totalCarsPassed1_EW=0;
    int totalCarsPassed2_EW=0;
    int carsInFront_NS=0;
    int carsInFront_EW=0;
    Coord* interPos = new Coord(600,450);
    //double serviceRate = 0.5; // 2.0 vehicles per second
    int deviceType;
    int receivedASL=0;
    int receivedASLBSM=0;
    int generatedASLs=0;
    int vehiclesInFront = 0; // will need to update this according to a loop detector around 300m away from intersection and one at intersection
    // if car reaches loop detector away from intersection, we increase
    // if car reaches loop detector at intersection, we decrease (they left)
    std::string lastvehIdNS1 = ""; // for lane area detector logic
    std::string lastvehIdNS2 = ""; // for lane area detector logic
    std::string lastvehIdEW1 = ""; // for lane area detector logic
    std::string lastvehIdEW2 = ""; // for lane area detector logic
    double globalMeanSpeed = 0.0;
    double globalMeanSpeedNS = 0.0;
    double globalMeanSpeedEW = 0.0;
    int totalExitedCarsEW = 0;
    int totalExitedCarsNS = 0;
    int totalStepsEW = 0;
    int totalStepsNS = 0;
    double altAvgFlowRateEW = 0.0;
    double altAvgFlowRateNS = 0.0;
    double totalWaitingTimeEW = 0.0;
    double totalWaitingTimeNS = 0.0;
    bool calledSpecialEW = false;
    bool calledSpecialNS = false;
    bool carsPassedInit = false;
    std::string filename = "";
    //std::list<std::string> lastStepLaneIds;
    std::map<std::string,std::string>* lastStepLaneIds = new std::map<std::string,std::string>(); // vehicle id -> current lane id
    std::map<long, double>* expArrivalTimes = new std::map<long, double>(); // l2 address -> expected arrival time
    std::map<std::string, int>* specialCarsEW = new std::map<std::string, int>(); // for the first cars in the 300 meters of the start up
    std::map<std::string, int>* specialCarsNS = new std::map<std::string, int>(); // for the first cars in the 300 meters of the start up
    TraCITrafficLightInterface* tlsInterface = new TraCITrafficLightInterface();
    TraCICommandInterface* traci;
    TraCICommandInterface::Trafficlight* tlsTraci;
    //Trafficlight* trafficLight;

};

} // namespace veins

