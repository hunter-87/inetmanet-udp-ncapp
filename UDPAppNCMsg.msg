//
// Copyright (C) 2012 Matteo Bruni
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, see <http://www.gnu.org/licenses/>.
//
cplusplus {{
#include "INETDefs.h"
}}

//
// Message class used by UDPEchoApp.
//
packet UDPAppNCMsg
{
    bool isRequest = true;
    // Network Coding Vars
    int coding_vector[];
    int payload_vector[];
   
    unsigned int generation;
    unsigned int total_pkt_num;		// number of message coded inside the payload
    
    
    bool isCombination = false;		// used in butterfly is true if the packet
    								// is generated in the STAR node

	string source;					// host name + udp app source
}

packet UDPAppNCMsg_Butterfly
{
    bool isCombination = false;
	unsigned char payload[];	   
    unsigned int generation;
    //simtime_t send_time;
    string source;
    

}

