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


package inet.applications.udpapp;

//
// Similar to UDPBasicApp, but it sends back the packet after reception.
//
simple UDPAppNC like UDPApp
{
    parameters:
        int localPort;
        int destPort;
        
        int generationSize = default(1);
        int FiniteFieldPower = default(4); 	// FF dimension = 2^FiniteFieldPower
        int random_seed = default(1456);
        int nTotalNodes = default(1);		// number of total nodes in the network
        string role = default("client"); 	// can be "client", "AP"
        								 	// or
        								 	// "startPoint" "STARPoint" "endPoint"
        								 
        bool butterfly_nc = default(true); 	// if false disable nc in butterfly network
        
        
        //bool butterfly_nc_multi_gen_size = default (false);
        									// if false each gen has only 1pkt
        									// if true each gen use generationSize
        
        string linked_AP_UDPApp = default("host[0].udpApp[0]");   
        									// used only if the node is a client
        									// it's the name of the AP node + udp app
        				
        string statistics_collector = default("host[0].udpApp[0]");   
        									// node that should calculate statistics
        									// used in butterfly network
        									
        // name of the start point nodes, used by star point to start a new generation
        string start_point_1 = default("host[0].udpApp[0]");
        string start_point_2 = default("host[0].udpApp[0]");
        					
        									
        
        volatile int messageLength @unit("B"); // length of messages to generate, int bytes
        volatile double messageFreq @unit("s"); // should usually be a random value, e.g. exponential(1)
        string destAddresses = default("host[0]"); // list of \IP addresses, separated by spaces
        @display("i=block/app");
    gates:
        input udpIn @labels(UDPControlInfo/up);
        output udpOut @labels(UDPControlInfo/down);
}



