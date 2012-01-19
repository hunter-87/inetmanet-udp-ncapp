//
// Copyright (C) 2005 Andras Babos
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


#ifndef __INET_UDPAPPNC_H
#define __INET_UDPAPPNC_H

#include <vector>
#include <omnetpp.h>
#include "ncutils.h"
//#include "UDPBasicApp.h"
#include "UDPAppBase.h"
#include "UDPAppNCMsg_m.h"

// table of:	idpacket -> uncodedpackets
// this map has [0 , blocknumber] entries
typedef std::map< int,  UncodedPacket*> payloadtable_t;

// table of uncoded packet decoded per source
// nb: we are working with 1 generation at time
typedef std::map<std::string, payloadtable_t > nctable_t;




typedef std::map<unsigned int, simtime_t> stat_t;//unsigned char*> nctable_t;

// table of decoder: 1 source <--> 1 decoder
typedef std::map<std::string, PacketDecoder *> decoder_t;//unsigned char*> nctable_t;


/**
 * UDP application. See NED for more info.
 */
class UDPAppNC : public UDPAppBase
{
  protected:

    std::string nodeName;
    std::string nodeRole;

	int localPort, destPort;
	std::vector<IPvXAddress> destAddresses;

	static int counter; // counter for generating a global number for each packet

	// statistics vars
	int numSent;
	int numReceived;
	int numDecodedPackets;	// total number of decoded packets in the entire simulation


	simtime_t gen_start_time;
	simtime_t gen_end_time;
    cOutVector GENERATIONTime;
    cOutVector PACKETTime;
    cOutVector BUTTERFLYGENERATIONTime;


	virtual int numInitStages() const {return 4;}

	virtual void initialize(int stage);
	virtual void handleMessage(cMessage *msg);
	virtual void finish();

	virtual void sendPacket();
	virtual void sendStartPointPacket();


	virtual cPacket *createPacket();
	virtual IPvXAddress chooseDestAddr();


	virtual void processPacket(cPacket *msg);

	virtual void starProcessPacket();//std::string source, payloadtable_t payload_table);

	virtual void endProcessPacket();
	CodedPacket* rebuildCodedPacketFromUDPMSG(UDPAppNCMsg * udp_msg);
    CodedPacket* createNewLinearComb();
    void createCodedPackets();



    //PacketDecoder* decoder_;

    FiniteField* ff;
    std::string combination_point; // used by endpoint save the name of the node that send linear combination
    int blockNumber; // number of input packets to lin combine
    int payloadLen;  // lenght of payload in bytes
    int random_seed;	// seed used to make linear combination
    //int combinationCounter;

    unsigned int generation;		// actual generation
    std::map<std::string, unsigned int> generationDecoded;  // save the last generation decoded
    std::map<std::string, unsigned int> packetsDecoded;		// counter of packets decoded up until now (used by clients)

    unsigned int currentGeneration;
    int numNetworkNodes;	// total number of net nodes (included AP)
    int ackCounter;			// counter of ack received (used by AP start and STAR)
	std::vector<CodedPacket*> codewords;	// actual set of msg to combine/decode
	void resetAckCounter();
	bool start_new_generation;	//used in butterfly net. start new generation
								// when all the endpoints have decoded all the packets

	bool star_send_packets;		// used to stop the packet generation from
								// STAR point

	std::vector<simtime_t> endpoint_start_time;

	nctable_t nc_table;			// table used to keep track of older pkgs
	stat_t statistics_table;
	decoder_t decoder_table;


  public:
	void incrementAckCounter();
	void saveStatistics(unsigned int gen, simtime_t generation_time);
    void startNewGeneration();

};

#endif




