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
#include "UDPBasicApp.h"
#include "UDPAppBase.h"


// nb the lenght of unsigned char is fixed in the .ini under messageLength
typedef std::map<unsigned int, UDPAppNCMsg_Butterfly*> nctable_t;//unsigned char*> nctable_t;
typedef std::map<unsigned int, simtime_t> stat_t;//unsigned char*> nctable_t;



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
	int numDecodedPackets;

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


	virtual void clientProcessPacket(cPacket *msg);
	virtual void starProcessPacket(cPacket *msg);
	virtual void endProcessPacket(cPacket *msg);

    CodedPacket* createNewLinearComb();

    void createCodedPackets();

    FiniteField* ff;

    PacketDecoder* decoder_;
    int blockNumber; // number of input packets to lin combine
    int payloadLen;  // lenght of payload in bytes
    int random_seed;	// seed used to make linear combination
    //int combinationCounter;
    unsigned int generation;		// actual generation
    unsigned int generationDecoded;  // save the last generation decoded
    int numNetworkNodes;	// total number of net nodes (included AP)
    int ackCounter;			// counter of ack received (used only by AP)
    int packetsDecoded;		// counter of packets decoded up until now (used by clients)
	std::vector<CodedPacket*> codewords;	// actual set of msg to combine/decode
	void resetAckCounter();

	nctable_t nc_table;
	stat_t statistics_table;


  public:
	void incrementAckCounter();

};

#endif




