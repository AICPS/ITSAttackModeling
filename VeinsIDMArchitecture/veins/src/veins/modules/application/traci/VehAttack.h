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
bool commAttackOn = true;
bool gpsAttackOn = false;
bool speedAttackOn = false;
bool signalAttackOn = false;
bool loopAttackOn = false;
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

} // namespace veins
