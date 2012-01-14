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


#include <omnetpp.h>
#include "UDPAppNC.h"
#include "ncutils.h"
#include "IPAddressResolver.h"
#include <math.h>

#include "UDPAppNCMsg_m.h"
#include "UDPControlInfo_m.h"


Define_Module(UDPAppNC);

int UDPAppNC::counter;

/*
 * Public method to increment the ack counter
 * when ackCounter reaches the number of clients (nTotal_nodes in .ned)
 * the AP will transmit the next message
 *
 */
void UDPAppNC::incrementAckCounter(){
	ackCounter++;
	if (ackCounter == numNetworkNodes -1){
		gen_end_time = simTime();
	}
	//std::cout << " NODE "<<getParentModule()->getFullName()<< " has increased the ack to "<<ackCounter <<std::endl;
}

/*
 * Public method to calculate statics
 * The node to be called it's the star Point since
 * there only one star point in the network
 */
void UDPAppNC::saveStatistics(unsigned int gen, simtime_t generation_time){


	stat_t::iterator it = statistics_table.find(gen);

	if (it == nc_table.end()){

		statistics_table[gen] = generation_time;//payload;

	} else {

		simtime_t previous_time = (*it).second;

		// save worst case
		if (generation_time < previous_time){
			BUTTERFLYGENERATIONTime.record(previous_time);
		} else {
			BUTTERFLYGENERATIONTime.record(generation_time);
		}

		nc_table.erase(it);
	}

}

void UDPAppNC::resetAckCounter(){
	gen_start_time = simTime();
	ackCounter = 0;
}

void UDPAppNC::initialize(int stage)
{
	// BASIC INIT

	// because of IPAddressResolver, we need to wait until interfaces are registered,
	// address auto-assignment takes place etc.
	if (stage!=3)
		return;


	// nc  init

    blockNumber = par("generationSize");
    payloadLen = par("messageLength");
    random_seed = par("random_seed");
    //combinationCounter = 0;

    numNetworkNodes = par("nTotalNodes");
    ackCounter = 0;
    decoder_ = 0;	// ptr to the decoder
    nodeRole = par("role").stringValue();//.str();
    packetsDecoded = 0;

    generation = 1;			// actual generation in the network
    generationDecoded = 0;	// generation up until all is decoded

	ff = new FiniteField(2, par("FiniteFieldPower"));

	std::cout << " RUOLO: <" <<nodeRole<<">" <<std::endl;

	if (nodeRole == "AP"){
		// create the first set of packets to send
		createCodedPackets();
	}

    // generate random numbers to use in lin comb
    srand(random_seed);



    PACKETTime.setName("UDP_PACKET_TIME");
    GENERATIONTime.setName("UDP_GENERATION_TIME");
    BUTTERFLYGENERATIONTime.setName("UDP_BUTTERFLY_GENERATION_TIME");

	counter = 0;
	numSent = 0;
	numReceived = 0;
	numDecodedPackets = 0;
	WATCH(numSent);
	WATCH(numReceived);

	localPort = par("localPort");
	destPort = par("destPort");


	const char *destAddrs = par("destAddresses");
	cStringTokenizer tokenizer(destAddrs);
	const char *token;
	while ((token = tokenizer.nextToken())!=NULL)
		destAddresses.push_back(IPAddressResolver().resolve(token));

	if (destAddresses.empty())
		return;

	bindToPort(localPort);

	cMessage *timer = new cMessage("sendTimer");
	scheduleAt((double)par("messageFreq"), timer);


	// END BASIC INIT
}

IPvXAddress UDPAppNC::chooseDestAddr()
{
    int k = intrand(destAddresses.size());
    return destAddresses[k];
}


/*
 * Deallocate dynamic memory
 */
void UDPAppNC::finish(){

    recordScalar("UDP_TOTALREC ", numReceived);
    recordScalar("UDP_TOTALDECREC ", numDecodedPackets);


	if (nodeRole == "AP"){

		for ( int i = 0 ; i < blockNumber ; i++) {
			delete codewords[i];
		}
	}
	if (nodeRole == "startPoint" or nodeRole == "STARPoint"){
		for (nctable_t::iterator it = nc_table.begin(); it != nc_table.end(); it++)
			delete (*it).second;

		nc_table.clear();

	}

	delete ff;

}

void UDPAppNC::createCodedPackets() {

    /*
     * Create unencoded packets
     */
    std::vector<UncodedPacket*> inputPackets;
	inputPackets.reserve(blockNumber);


	//std::cout << " CREATING NEW SET OF CODEWORDS "<< std::endl;

	for ( int i = 0 ; i < blockNumber ; i++) {
		unsigned char* payload = new unsigned char[payloadLen];

		std::fill(payload, payload+payloadLen, 0xA0+i+generation);
		inputPackets.push_back(new UncodedPacket(i, payload, payloadLen));
		//std::cout<< "Uncodedpacket: " << i << ": "<< inputPackets[i]->toString()<< std::endl;

		// free memory since uncodedpacket() creates a copy
		delete [] payload;
	}

	if (codewords.size() != 0) {
		for ( int i = 0 ; i < blockNumber ; i++) {
			delete codewords[i];
		}
		codewords.clear();
	}

	/* prepare the input packets to be sent on the network */
	codewords.reserve(blockNumber); //= new CodedPacket[blockNumber];

	//std::cout << " Coded Packet: " <<std::endl;
	for ( int i = 0 ; i < blockNumber ; i++) {

		codewords.push_back(new CodedPacket( inputPackets[i], blockNumber, ff));
		delete inputPackets[i];
		//std::cout<< "Packet: " << i << ": "<< codewords[i]->toString()<< std::endl;


	}

	generation++;
}


void UDPAppNC::sendPacket(){

    cPacket *payload = createPacket();
    //IPvXAddress destAddr = chooseDestAddr();
    IPvXAddress destAddr = IPAddress::ALLONES_ADDRESS;
    sendToUDP(payload, localPort, destAddr, destPort);

    numSent++;
}

void UDPAppNC::sendStartPointPacket() {

	// create packet
	char msgName[32];
	sprintf(msgName,"UDPNC-BF-%d", counter++);

	UDPAppNCMsg_Butterfly *message = new UDPAppNCMsg_Butterfly(msgName);

	message->setPayloadArraySize(payloadLen);
	for (int i=0; i<payloadLen;i++){
		message->setPayload(i, 0xA0+generation);
	}

	message->setGeneration(generation);
	generation++;

	message->setByteLength(par("messageLength").longValue());
	// flag that this packet is a combination
	message->setIsCombination(true);

	message->setSend_time(simTime());

	cPacket *packet = message;
    IPvXAddress destAddr = IPAddress::ALLONES_ADDRESS;
    sendToUDP(packet, localPort, destAddr, destPort);

	numSent++;
}


CodedPacket* UDPAppNC::createNewLinearComb(){

	CodedPacket* combination = CodedPacket::createEmptyCodedPacketPtr(blockNumber, payloadLen, ff);

	for ( int j = 0 ; j < blockNumber ; j++) {
		int x = rand()%ff->getCardinality(); // r.nextInt(ff.getCardinality());
		CodedPacket* copy = codewords[j]->scalarMultiply(x);
		combination->addInPlace(copy);
		delete copy;
	}

	return combination;
}


cPacket* UDPAppNC::createPacket()
{

	// se sono un sender

    char msgName[32];
    sprintf(msgName,"UDPNC-%d", counter++);

    UDPAppNCMsg *message = new UDPAppNCMsg(msgName);

    CodedPacket* packet = createNewLinearComb();

    // set the coding vector
	int cv_size_ = packet->getCodingVector()->getLength();
	message->setCoding_vectorArraySize(cv_size_);
	for(int k=0;k<cv_size_;k++){
		message->setCoding_vector(k, packet->getCodingVector()->getCoordinate(k));
	}

	// set the payload vector
	int pv_size_ = packet->getPayloadVector()->getLength();
	message->setPayload_vectorArraySize(pv_size_);
	for(int k=0;k<pv_size_;k++){
		message->setPayload_vector(k, packet->getPayloadVector()->getCoordinate(k) );
	}

	message->setTotal_pkt_num(blockNumber);
	message->setGeneration(generation);

    // TODO: calcola dimensione coding vector + dimensione payload vector
	// 		 per ottenere la dimensione reale
    message->setByteLength(par("messageLength").longValue());

    return message;
}


void UDPAppNC::handleMessage(cMessage *msg)
{
    if (msg->isSelfMessage())
    {
    	EV << " SELF MESSAGE " <<endl;
    	// check the role of this node
		if (nodeRole == "AP"){

			if(ackCounter == numNetworkNodes - 1){ // -1 since the AP is included in the counter

				resetAckCounter();

				//std::cout << " Received "<<ackCounter<<" ack. creating new set to send "<<std::endl;
				// if we have all the acks we need to create another set of codewords
				createCodedPackets();

				GENERATIONTime.record( fabs(gen_start_time - gen_end_time));
				PACKETTime.record(fabs(gen_start_time - gen_end_time)/double(blockNumber));

				//std::cout<<" GENERATION " <<fabs(gen_end_time - gen_start_time)<<std::endl;
				//std::cout<<" PACKET " <<fabs(gen_end_time - gen_start_time)/double(blockNumber)<<std::endl;

			}

			//std::cout<< " Invio nuova combinazione lineare"<<std::endl;

			// send a new linear combination from the codewords set
			sendPacket();
		}

		if (nodeRole == "startPoint"){

			// crea nuovo pacchetto con generazione nuova e manda in broadcast
			// send a new linear combination from the codewords set
			sendStartPointPacket();
		}

        // schedule next message
        scheduleAt(simTime()+(double)par("messageFreq"), msg);
    }
    else
    {
		//std::cout<< " client ha ricevuto un pacchetto"<<std::endl;

    	if (nodeRole == "client"){
			// process incoming packet
    		//std::cout<< "Nodo" << getParentModule()->getFullName()<< " client ha ricevuto un pacchetto"<<std::endl;
			clientProcessPacket(PK(msg));
    	}
    	else if (nodeRole == "STARPoint"){
    		starProcessPacket(PK(msg));
    	}
    	else if (nodeRole == "endPoint"){
    		endProcessPacket(PK(msg));
    	} else {
    	    delete msg;
    	}

    }

    if (ev.isGUI())
    {
        char buf[40];
        sprintf(buf, "rcvd: %d pks\nsent: %d pks", numReceived, numSent);
        getDisplayString().setTagArg("t",0,buf);
    }
}



void UDPAppNC::clientProcessPacket(cPacket *msg)
{
    if (msg->getKind() == UDP_I_ERROR){
        delete msg;
        return;
    }

    UDPAppNCMsg *udp_msg = check_and_cast<UDPAppNCMsg *>(msg);

    if (udp_msg==NULL){
    	delete msg;
    	return;
    }

    if (udp_msg->getGeneration() <= generationDecoded){
    	//std::cout << "this generation has already been decode. skipping" <<std::endl;
    	delete msg;
    	return;
    }

	// this ptr be deleted when the codedpacket will be deleted
	FiniteFieldVector* ff_cv_ = new FiniteFieldVector(udp_msg->getCoding_vectorArraySize(), ff);
	for(unsigned int i = 0; i<udp_msg->getCoding_vectorArraySize();i++){
		ff_cv_->setCoordinate(i,udp_msg->getCoding_vector(i));
	}
	// this ptr be deleted when the codedpacket will be deleted
	FiniteFieldVector* ff_pv_ = new FiniteFieldVector(udp_msg->getPayload_vectorArraySize(), ff);
	for(unsigned int i = 0; i<udp_msg->getPayload_vectorArraySize();i++){
		ff_pv_->setCoordinate(i,udp_msg->getPayload_vector(i));
	}

	// recreate the coded packet
    CodedPacket* coded_packet_ = new CodedPacket(ff_cv_, ff_pv_);

	// now it's time to decode
	// if we have never instantiated the decoder
	if (decoder_ == 0) {
		decoder_ = new PacketDecoder(ff, blockNumber, payloadLen);
	}

	// try to decode the packet received
	std::vector<UncodedPacket*> uncoded_pkts_ = decoder_->addPacket(coded_packet_);

	// count the number of packets decoded up until now
	packetsDecoded = packetsDecoded + uncoded_pkts_.size();

	numDecodedPackets = numDecodedPackets + uncoded_pkts_.size();

	//std::cout << " NODE "<<getParentModule()->getFullName()<< " has decoded "<<packetsDecoded <<std::endl;

	if (packetsDecoded == blockNumber) {
		// we have decoded all the packets so we have to increase the ack
		// in the AP, reset the decoder

		// save the generation decoded
		generationDecoded = udp_msg->getGeneration();

		// hacky code
		// increase the ack
		check_and_cast<UDPAppNC *>(getParentModule()->getParentModule()->getModuleByRelativePath(par("linked_AP_UDPApp").stringValue()))->incrementAckCounter();


		packetsDecoded = 0 ;
		delete decoder_;

		decoder_ = 0;
	}

	for (unsigned int i = 0 ; i < uncoded_pkts_.size() ; i++) {
			delete uncoded_pkts_[i];
	}

    numReceived++;

    delete msg;
}

void UDPAppNC::starProcessPacket(cPacket *msg){

	if (msg->getKind() == UDP_I_ERROR){
		delete msg;
		return;
	}

	if (par("butterfly_nc").boolValue() == false){
		// broadcast the packet

	    IPvXAddress destAddr = IPAddress::ALLONES_ADDRESS;
	    sendToUDP(msg, localPort, destAddr, destPort);
	    numSent++;

		return;
	}

	UDPAppNCMsg_Butterfly *udp_msg = check_and_cast<UDPAppNCMsg_Butterfly *>(msg);

	if (udp_msg==NULL){
		delete msg;
		return;
	}


	// check if it's the first packet of this generation
	nctable_t::iterator it = nc_table.find(udp_msg->getGeneration());

	if (it == nc_table.end()){

		//no entry for this generation => we save this packet
		nc_table[udp_msg->getGeneration()] = udp_msg;

		// TODO: Timer per delete

	} else {

		// this is the second packet of this generation.
		// we need to do an XOR between the two pkts.

		// unsigned char* previous_payload = (*it).second;
		UDPAppNCMsg_Butterfly *previous_msg = (*it).second;

		unsigned char* result = new unsigned char[payloadLen];

		// get the result of: payload XOR previous_payload
		for(int i = 0 ; i< payloadLen; i++){
			result[i] = udp_msg->getPayload(i)^previous_msg->getPayload(i);
		}


		// create packet
		char msgName[32];
		sprintf(msgName,"UDPNC-BF-%d", counter++);

		UDPAppNCMsg_Butterfly *message = new UDPAppNCMsg_Butterfly(msgName);

		message->setPayloadArraySize(payloadLen);
		for (int i=0; i<payloadLen;i++){
			message->setPayload(i, result[i]);
		}

		message->setGeneration(udp_msg->getGeneration());
		message->setByteLength(par("messageLength").longValue());
		// flag that this packet is a combination
		message->setIsCombination(true);

		// propagate start time only of the first message sended
		if(previous_msg->getSend_time() < udp_msg->getSend_time() ){
			message->setSend_time(previous_msg->getSend_time());
		} else {
			message->setSend_time(udp_msg->getSend_time());
		}


		cPacket *packet = message;
		IPvXAddress destAddr = IPAddress::ALLONES_ADDRESS;
		sendToUDP(packet, localPort, destAddr, destPort);

		EV << " sending linear combination from STARPoint"<<endl;

		numSent++;
		// and send it

		delete previous_msg;
		delete udp_msg;
		nc_table.erase(it);

	}


}

void UDPAppNC::endProcessPacket(cPacket *msg){
	if (msg->getKind() == UDP_I_ERROR){
		delete msg;
		return;
	}

	UDPAppNCMsg_Butterfly *udp_msg = check_and_cast<UDPAppNCMsg_Butterfly *>(msg);

	if (udp_msg==NULL){
		delete msg;
		return;
	}


	// check if it's the first packet of this generation
	nctable_t::iterator it = nc_table.find(udp_msg->getGeneration());

	if (it == nc_table.end()){

		//no entry for this generation => we save this packet
		nc_table[udp_msg->getGeneration()] = udp_msg;//payload;

		// TODO: Timer per delete

	} else {

		UDPAppNCMsg_Butterfly *previous_msg = (*it).second;

		if (par("butterfly_nc").boolValue() == false){

			// we have all the packets since there is no retransmission

			// print the two packets for diagnostics


		} else {


			// this is the second packet of this generation.
			// we need to do an XOR between the two pkts.

			unsigned char* result = new unsigned char[payloadLen];

			// get the result of: payload XOR previous_payload
			// result is the missing packet
			for(int i = 0 ; i< payloadLen; i++){
				result[i] = udp_msg->getPayload(i)^previous_msg->getPayload(i);
			}

			// print the two packets for diagnostics

			delete [] result;

		}

		simtime_t start_time = (udp_msg->getSend_time() < previous_msg->getSend_time()) ? udp_msg->getSend_time() : previous_msg->getSend_time();

		check_and_cast<UDPAppNC *>(getParentModule()->getParentModule()->getModuleByRelativePath(par("statistics_collector").stringValue()))->saveStatistics(udp_msg->getGeneration(),fabs(simTime() - start_time));


		delete udp_msg;
		delete previous_msg;
		nc_table.erase(it);


	}
}

