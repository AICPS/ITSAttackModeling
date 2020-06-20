//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

cplusplus {{
#include "veins/base/utils/SimpleAddress.h"
#include "veins/modules/messages/BaseFrame1609_4_m.h"
}}
//packet BaseFrame1609_4;
class BaseFrame1609_4;
class LAddress::L2Type extends void;
//
// TODO generated message class
//
packet ASLMessage extends BaseFrame1609_4 {
    double aslSpeed = 0.0;
   	LAddress::L2Type recipientAddress = -1;
}
