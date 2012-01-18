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
#include <algorithm>

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
 * Public method to enable a new generation
 * used after all the endpoints have received all the packets
 *
 */
void UDPAppNC::startNewGeneration(){
	std::cout << " START NEW GENERATION" << std::endl;
	if (nodeRole == "startPoint"){
		start_new_generation = true;
	}
}

/*
 * Public method to calculate statics
 * The node to be called it's the star Point since
 * there only one star point in the network
 */
void UDPAppNC::saveStatistics(unsigned int gen, simtime_t generation_time){
	// TODO: warn start point to start a new generation

	std::cout << "Save Statitstics chiamato con generazione "<< gen << std::endl;
	stat_t::iterator it = statistics_table.find(gen);

	if (it == statistics_table.end()){

		statistics_table[gen] = generation_time;//payload;

	} else {

		simtime_t previous_time = (*it).second;
		std::cout << "PRimo Tempo "<<generation_time << " Secondo tempo " <<previous_time ;

		// save worst case
		if (generation_time < previous_time){
			std::cout << " Scelgo "<< ( previous_time)<< std::endl;
			BUTTERFLYGENERATIONTime.record(previous_time);
		} else {
			std::cout << " Scelgo "<< (generation_time )<< std::endl;
			BUTTERFLYGENERATIONTime.record(generation_time);
		}

		statistics_table.erase(it);

		std::cout << " Start new Generation " <<std::endl;
		// start a new generation
		check_and_cast<UDPAppNC *>(getParentModule()->getParentModule()->getModuleByRelativePath(par("start_point_1").stringValue()))->startNewGeneration();
		check_and_cast<UDPAppNC *>(getParentModule()->getParentModule()->getModuleByRelativePath(par("start_point_2").stringValue()))->startNewGeneration();

	}

}

void UDPAppNC::resetAckCounter(){
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
//    decoder_ = 0;	// ptr to the decoder
//    decoder_b = 0;
    nodeRole = par("role").stringValue();//.str();
    packetsDecoded = 0;
    start_new_generation = true;	//used in butterfly network
    star_send_packets = false;
    generation = 0;			// actual generation in the network

	ff = new FiniteField(2, par("FiniteFieldPower"));

	EV << " Node Role : <" <<nodeRole<<">" <<std::endl;

	if (nodeRole == "AP" or nodeRole == "startPoint"){
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


	if (nodeRole == "AP" or nodeRole == "startPoint"){

		for ( int i = 0 ; i < blockNumber ; i++) {
			delete codewords[i];
		}
	}
	if (nodeRole == "startPoint" or nodeRole == "STARPoint" or nodeRole == "endPoint"){
		for (nctable_t::iterator it = nc_table.begin(); it != nc_table.end(); it++){
			payloadtable_t& inner_table = (*it).second;
			for (payloadtable_t::iterator it2 = inner_table.begin(); it2 != inner_table.end(); it2++){
				delete (*it2).second;
			}
			inner_table.clear();
		}
		nc_table.clear();
	}
//		for (unsigned int i = 0; i < ((*it).second).size(); i++)
//			delete (*it).second[i];

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

		std::fill(payload, payload+payloadLen, 0x00+(i+generation)%255);
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

	gen_start_time = simTime();
	generation++;
}


void UDPAppNC::sendPacket(){

    cPacket *payload = createPacket();
    //IPvXAddress destAddr = chooseDestAddr();
    IPvXAddress destAddr = IPAddress::ALLONES_ADDRESS;

	if (nodeRole == "AP")
		sendToUDP(payload, localPort, destAddr, destPort);
	else
		sendToUDPDelayed(payload, localPort, destAddr, destPort);

    numSent++;

    std::cout << getFullPath() << " sta inviando gen:"<<generation<<std::endl;
}

void UDPAppNC::sendStartPointPacket() {

	//cPacket *payload = createPacket();


	//createCodedPackets();


	/////////////////////////////////////////////////////
//	// create packet
//	char msgName[32];
//	sprintf(msgName,"UDPNC-BF-%d", counter++);
//
//	UDPAppNCMsg_Butterfly *message = new UDPAppNCMsg_Butterfly(msgName);
//
//	//std::cout << "Sending packet from generation: "<<generation << ": "<< std::endl;
//	message->setPayloadArraySize(payloadLen);
//	for (int i=0; i<payloadLen;i++){
//		message->setPayload(i, 0x00+(generation%255));
//		//std::cout <<std::endl<< std::hex << message->getPayload(i) << " ";
//	}
//	//std::cout << std::endl;
//
//	message->setGeneration(generation);
//
//	message->setByteLength(par("messageLength").longValue());
//	// flag that this packet is a combination
//	message->setIsCombination(false);
//
//	//message->setSend_time(simTime());
//
//	//std::cout<< "\n\n "<< getFullPath().c_str() <<std::endl;
//	message->setSource(getFullPath().c_str());
//
//	cPacket *packet = message;
//    IPvXAddress destAddr = IPAddress::ALLONES_ADDRESS;
//    sendToUDP(packet, localPort, destAddr, destPort);
//
//    std::cout << " ## start point invio ora generazione "<< generation <<" time: " << simTime()<< std::endl;
//
//	generation = generation +1;
//    numSent++;
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
	//getNedTypeName();
	std::string hostname(getParentModule()->getFullName());
	std::string appname(getFullName());

	std::string source = hostname + "." + appname;
	message->setSource(source.c_str()); //getFullPath().c_str());

	if (nodeRole == "STARPoint") {
		message->setIsCombination(true);
	} else {
		message->setIsCombination(false);
	}

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

        // schedule next message
        scheduleAt(simTime()+(double)par("messageFreq"), msg);

    	// check the role of this node
		if ( nodeRole == "AP" or nodeRole == "startPoint" or nodeRole == "STARPoint") {

			EV << " AP ENTRATO " <<endl;

			if (ackCounter == numNetworkNodes - 1){ // -1 since the AP is included

				// when a star point has all the acks from the endpoints, it stops itself
				if (nodeRole == "STARPoint" ){
					star_send_packets = false;
					return;
				}

				GENERATIONTime.record( fabs(gen_start_time - gen_end_time));
				PACKETTime.record(fabs(gen_start_time - gen_end_time)/double(blockNumber));

				resetAckCounter();

				//std::cout << " Received "<<ackCounter<<" ack. creating new set to send "<<std::endl;
				//std::cout<<" GENERATION " <<fabs(gen_end_time - gen_start_time)<<std::endl;
				//std::cout<<" PACKET " <<fabs(gen_end_time - gen_start_time)/double(blockNumber)<<std::endl;

				// stop the start point from sending another simulation
				// until the end points had decoded everything
				start_new_generation = false;

				// if we have all the acks we need to create another set of codewords
				createCodedPackets();

			}

			if (nodeRole == "startPoint" and start_new_generation == false)
				return;

			if ((star_send_packets == false) and (nodeRole == "STARPoint"))
				return;

			EV << " AP INVIA " << endl;

			// send a new linear combination from the codewords set
			sendPacket();
		}

//		if (nodeRole == "startPoint"){
//
//			// crea nuovo pacchetto con generazione nuova e manda in broadcast
//			// send a new linear combination from the codewords set
//			sendStartPointPacket();
//		}

    }
    else
    {
		//std::cout<< "\n\n\n NODE ha ricevuto un pacchetto \n\n"<<std::endl;
    	if (nodeRole == "client" or nodeRole == "STARPoint" or nodeRole == "endPoint"){
    		processPacket(PK(msg));
    	} else {
    	    delete msg;
    	}


//    	if (nodeRole == "client"){
//			// process incoming packet
//    		//std::cout<< "Nodo" << getParentModule()->getFullName()<< " client ha ricevuto un pacchetto"<<std::endl;
//    		processPacket(PK(msg));
//    	}
//    	else if (nodeRole == "STARPoint"){
//    		//std::cout<< "\n\n\n STAR ha ricevuto un pacchetto \n\n"<<std::endl;
//
//    		processPacket(PK(msg));
//    	}
//    	else if (nodeRole == "endPoint"){
//    		//std::cout<< "\n\n\n END ha ricevuto un pacchetto \n\n"<<std::endl;
//
//    		endProcessPacket(PK(msg));
//    	} else {
//    	    delete msg;
//    	}

    }

    if (ev.isGUI())
    {
        char buf[40];
        sprintf(buf, "rcvd: %d pks\nsent: %d pks", numReceived, numSent);
        getDisplayString().setTagArg("t",0,buf);
    }
}

CodedPacket* UDPAppNC::rebuildCodedPacketFromUDPMSG(UDPAppNCMsg * udp_msg){

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
    return new CodedPacket(ff_cv_, ff_pv_);
}

void UDPAppNC::processPacket(cPacket *msg) {

    if (msg->getKind() == UDP_I_ERROR){
        delete msg;
        return;
    }

    UDPAppNCMsg *udp_msg = check_and_cast<UDPAppNCMsg *>(msg);

    if (udp_msg==NULL){
    	delete msg;
    	return;
    }

    // if this is a STAR point and nc is off retransmit without process
	if (nodeRole == "STARPoint" and par("butterfly_nc").boolValue() == false){
		// broadcast the packet

	    IPvXAddress destAddr = IPAddress::ALLONES_ADDRESS;
	    UDPAppNCMsg* copy = check_and_cast<UDPAppNCMsg *>(msg->dup());
	    copy->setIsCombination(true);
	    sendToUDP(copy, localPort, destAddr, destPort);
	    numSent++;
	    delete msg;
		return;
	}

	// save start time for statistical use
	if (nodeRole == "endPoint")
		endpoint_start_time.push_back(udp_msg->getCreationTime());

    // recover the source from the packet
    std::string source(udp_msg->getSource());

    if (udp_msg->getGeneration() <= generationDecoded[source]){
    	std::cout << "Node: "<<getFullPath()<<" packet from :"<<source<<" this generation has already been decoded. skipping" <<std::endl;
    	delete msg;
    	return;
    }
    currentGeneration = udp_msg->getGeneration();

    CodedPacket* coded_packet_ = rebuildCodedPacketFromUDPMSG(udp_msg);

	// get the associated decoder
	decoder_t::iterator it = decoder_table.find(source);
	if (it == decoder_table.end()){
		// create if it's new
		decoder_table[source] = new PacketDecoder(ff, blockNumber, payloadLen);
	}

	// save the source of combinated packets for later use in
	// endpointprocess
	if (udp_msg->getIsCombination())
		combination_point = source;


	PacketDecoder* decoder_ = decoder_table[source];


//	// now it's time to decode
//	// if we have never instantiated the decoder
//	if (decoder_ == 0) {
//		decoder_ = new PacketDecoder(ff, blockNumber, payloadLen);
//	}

	// try to decode the packet received
	std::vector<UncodedPacket*> uncoded_pkts_ = decoder_->addPacket(coded_packet_);

	for (unsigned int i = 0; i < uncoded_pkts_.size(); i++ ) {
		int id = uncoded_pkts_[i]->getId();
		nc_table[source][id] = uncoded_pkts_[i];
	}

//	// if it's the first time for this generation
//	nctable_t::iterator it2 = nc_table.find(source);
//	if (it2 == nc_table.end()){
//		nc_table[source] = uncoded_pkts_;
//	} else {
//		// concatenate
//		nc_table[source].insert(nc_table[source].end(), uncoded_pkts_.begin(), uncoded_pkts_.end());
//	}

	// count the number of packets decoded up until now
	packetsDecoded = packetsDecoded + uncoded_pkts_.size();
	// total number of packets
	numDecodedPackets = numDecodedPackets + uncoded_pkts_.size();

	//std::cout << " NODE "<<getParentModule()->getFullName()<< " has decoded "<<packetsDecoded <<std::endl;

	if (packetsDecoded == blockNumber) {
		// we have decoded all the packets so we have to increase the ack
		// in the AP, reset the decoder

		// save the generation decoded
		generationDecoded[source] = udp_msg->getGeneration();

		// hacky code
		// increase the ack
		//check_and_cast<UDPAppNC *>(getParentModule()->getParentModule()->getModuleByRelativePath(par("linked_AP_UDPApp").stringValue()))->incrementAckCounter();
		//EV << "PRECRASH !! :" << source.c_str()<<":" << endl;
		check_and_cast<UDPAppNC *>(getParentModule()->getParentModule()->getModuleByRelativePath(source.c_str()))->incrementAckCounter();
		//check_and_cast<UDPAppNC *>(getModuleByRelativePath(source.c_str()))->incrementAckCounter();

		packetsDecoded = 0 ;
		delete decoder_;
		decoder_table.erase(source);

		// TODO: if star processa gli uncoded packets
		// esegui xor e ritrasmetti
		if (nodeRole == "STARPoint"){
			// tell the starpoint which generation has to send
			generation = udp_msg->getGeneration();
			std::cout << "Decodificato pacchetti per : "<<source <<std::endl;
			starProcessPacket();//source, nc_table[source]);
		}
		if (nodeRole == "endPoint"){
			endProcessPacket();
		}

		if (nodeRole == "client"){

			// the node is a simple node, clear the table since it's not used anymore

			for (unsigned int i = 0 ; i < nc_table[source].size() ; i++) {
				delete nc_table[source][i];
			}

			nc_table.erase(source);

		}

	}

    numReceived++;

    delete msg;

}

void UDPAppNC::starProcessPacket() { //std::string source, payloadtable_t payload_table){
	std::cout << " Arrivato a STAR tablesize:"<<nc_table.size()<<std::endl;
	// we need 2 elements to decode
	if( nc_table.size() != 2){
		return;
	} else {

		// check if we have decoded all the packets for each source
		for(nctable_t::iterator it = nc_table.begin(); it != nc_table.end(); it++) {
			if ((*it).second.size() != blockNumber)
				return;
		}


		// create new coded Packets

		/*
		 * Create unencoded packets
		 */
		std::vector<UncodedPacket*> inputPackets;
		inputPackets.reserve(blockNumber);

		for ( int i = 0 ; i < blockNumber ; i++) {

			unsigned char* payload = new unsigned char[payloadLen];
			// fill with 0 since A Xor 0 = A
			std::fill(payload, payload+payloadLen, 0x00);

			for(nctable_t::iterator it = nc_table.begin(); it != nc_table.end(); it++) {

				//payloadtable_t& inner_table = (*it).second;
				unsigned char* temp = new unsigned char[payloadLen];
				// get the i-element from the inner table
				memcpy(temp, (*it).second[i]->getPayload(), (*it).second[i]->getPayloadLength());

				// do bitwise XOR
				for ( int i = 0; i < payloadLen; i++ )
					payload[i] = payload[i] ^ temp[i];

				delete [] temp;
			}


			inputPackets.push_back(new UncodedPacket(i, payload, payloadLen));
			//std::cout<< "Uncodedpacket: " << i << ": "<< inputPackets[i]->toString()<< std::endl;

			// free memory since uncodedpacket() creates a copy
			delete [] payload;

		}
		// clear codewords if it's not empty
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

		// mark to true to enable packet forwarding
		star_send_packets = true;

		// clear memory
		for (nctable_t::iterator it = nc_table.begin(); it != nc_table.end(); it++){
			payloadtable_t& inner_table = (*it).second;
			for (payloadtable_t::iterator it2 = inner_table.begin(); it2 != inner_table.end(); it2++){
				delete (*it2).second;
			}
			inner_table.clear();
		}
		nc_table.clear();

		//generation++;

	}

}





//	std::cout << " ## STAR point arriva ora generazione "<< generation <<" time: " << simTime()<< std::endl;

//	if (msg->getKind() == UDP_I_ERROR){
//		delete msg;
//		return;
//	}
//
//	if (par("butterfly_nc").boolValue() == false){
//		// broadcast the packet
//
//	    IPvXAddress destAddr = IPAddress::ALLONES_ADDRESS;
//	    UDPAppNCMsg_Butterfly* copy = check_and_cast<UDPAppNCMsg_Butterfly *>(msg->dup());
//	    copy->setIsCombination(true);
//	    sendToUDP(copy, localPort, destAddr, destPort);
//	    numSent++;
//	    delete msg;
//		return;
//	}
//
//
//	UDPAppNCMsg_Butterfly *udp_msg = check_and_cast<UDPAppNCMsg_Butterfly *>(msg);
//
//	if (udp_msg==NULL){
//		delete msg;
//		return;
//	}


//	// check if it's the first packet of this generation
//	nctable_t::iterator it = nc_table.find(udp_msg->getGeneration());
//
//	if (it == nc_table.end()){
//
//		//no entry for this generation => we save this packet
//		nc_table[udp_msg->getGeneration()] = udp_msg;
//
//		// TODO: Timer per delete
//
//	} else {
//
//		// this is the second packet of this generation.
//		// we need to do an XOR between the two pkts.
//
//		// unsigned char* previous_payload = (*it).second;
//		UDPAppNCMsg_Butterfly *previous_msg = (*it).second;
//
//		unsigned char* result = new unsigned char[payloadLen];
//
//		// get the result of: payload XOR previous_payload
//		for(int i = 0 ; i< payloadLen; i++){
//			result[i] = udp_msg->getPayload(i)^previous_msg->getPayload(i);
//		}
//
//
//		// create packet
//		char msgName[32];
//		sprintf(msgName,"UDPNC-BF-%d", counter++);
//
//		UDPAppNCMsg_Butterfly *message = new UDPAppNCMsg_Butterfly(msgName);
//
//		message->setPayloadArraySize(payloadLen);
//		for (int i=0; i<payloadLen;i++){
//			message->setPayload(i, result[i]);
//		}
//
//		message->setGeneration(udp_msg->getGeneration());
//		message->setByteLength(par("messageLength").longValue());
//		// flag that this packet is a combination
//		message->setIsCombination(true);
//
////		// propagate start time only of the first message sended
////		if(previous_msg->getSend_time() < udp_msg->getSend_time() ){
////			message->setSend_time(previous_msg->getSend_time());
////		} else {
////			message->setSend_time(udp_msg->getSend_time());
////		}
//
//		cPacket *packet = message;
//		IPvXAddress destAddr = IPAddress::ALLONES_ADDRESS;
//		sendToUDP(packet, localPort, destAddr, destPort);
//
//		EV << " sending linear combination from STARPoint"<<endl;
//
//		numSent++;
//		// and send it
//
//		delete previous_msg;
//		delete udp_msg;
//		nc_table.erase(it);
//
//	}




void UDPAppNC::endProcessPacket() {
	std::cout << "NODO: "<< getFullPath() <<" PACCHETTO ARRIVATO A END table size: "<<nc_table.size() << std::endl;
	// we need 2 elements to decode
	if (nc_table.size() != 2) {
		return;
	} else {

		// check if we have decoded all the packets for each source
		for(nctable_t::iterator it = nc_table.begin(); it != nc_table.end(); it++) {
			if ((*it).second.size() != blockNumber)
				return;
		}

		// if network coding is false we have already finished
		if (par("butterfly_nc").boolValue() == false){
			std::cout << " Arrivati tutti i pacchetti " << std::endl;
		} else {

			// get the decoded payloads

				// TODO: XOR
		}

		// save statistics
		simtime_t start_time = *(std::min_element( endpoint_start_time.begin(), endpoint_start_time.end() ));
		// send the interval to the statistic collector
		check_and_cast<UDPAppNC *>(getParentModule()->getParentModule()->getModuleByRelativePath(par("statistics_collector").stringValue()))->saveStatistics(currentGeneration, fabs(simTime() - start_time));


		// simtime_t start_time = (udp_msg->getCreationTime() < previous_msg->getCreationTime()) ? udp_msg->getCreationTime() : previous_msg->getCreationTime();
		// send to the statistic collector
		std::cout << " # "<<getFullPath()<<" Salvo " << fabs(simTime() - start_time) << std::endl;

		// clear nctable_t
		for (nctable_t::iterator it = nc_table.begin(); it != nc_table.end(); it++){
			payloadtable_t& inner_table = (*it).second;
			for (payloadtable_t::iterator it2 = inner_table.begin(); it2 != inner_table.end(); it2++){
				delete (*it2).second;
			}
			inner_table.clear();
		}
		nc_table.clear();
	}
}


//	if (msg->getKind() == UDP_I_ERROR){
//		delete msg;
//		return;
//	}
//
//	UDPAppNCMsg_Butterfly *udp_msg = check_and_cast<UDPAppNCMsg_Butterfly *>(msg);
//
//	if (udp_msg==NULL){
//		delete msg;
//		return;
//	}

//	std::cout<< "\t\t\t ## end [  "<< getFullPath() << "] arriva generazione "<< udp_msg->getGeneration() << " tempo "<< simTime()<<" mittente "<< udp_msg->getSource()<< " iscomb "<<udp_msg->getIsCombination() <<std::endl;
//
//	// check if it's the first packet of this generation
//	nctable_t::iterator it = nc_table.find(udp_msg->getGeneration());
//
//	if (it == nc_table.end()){
//
//		//no entry for this generation => we save this packet
//		nc_table[udp_msg->getGeneration()] = udp_msg->dup();//payload;
//
//		delete msg;
//		// TODO: Timer per delete
//
//	} else {
//
//
//		UDPAppNCMsg_Butterfly *previous_msg = (*it).second;
//
//		// check if it isn't the same package
//
////		std::cout << " *********** Siamo al secondo pacchetto ***********"<<std::endl;;
////		std::cout << " *** pacchetto salvato: gen "<<previous_msg->getGeneration()<<" sorgente :"<<previous_msg->getSource()<<":"<<std::endl;
////		std::cout << " *** pacchetto arrivato: gen "<<udp_msg->getGeneration()<<" sorgente :"<<udp_msg->getSource()<<":"<<std::endl;
//
//
//
//		if (strcmp(udp_msg->getSource(), previous_msg->getSource()) == 0){
//			//std::cout << "Sono uguali "<<std::endl;
//			//std::cout<< "MALE mi è arrivato " << udp_msg->getSource() << " = "<< previous_msg->getSource() << std::endl;
//			delete msg;
//			return;
//		} else {
//			std::cout << "Sono diversi "<<std::endl;
//			//std::cout<< "OK mi è arrivato " << udp_msg->getSource() << " != "<< previous_msg->getSource() << std::endl;
//		}
//
//
//		bubble("Arrivati Entrambi ");
//
//		if (par("butterfly_nc").boolValue() == false){
//
//			// we have all the packets since there is no retransmission
//
//			// print the two packets for diagnostics
//			unsigned char* result = new unsigned char[payloadLen];
//
//			// get the result of: payload XOR previous_payload
//			// result is the missing packet
//			for(int i = 0 ; i< payloadLen; i++){
//				result[i] = udp_msg->getPayload(i);
//			}
//
//		} else {
//
//
//			// this is the second packet of this generation.
//			// we need to do an XOR between the two pkts.
//
//			unsigned char* result = new unsigned char[payloadLen];
//
//			// get the result of: payload XOR previous_payload
//			// result is the missing packet
//			for(int i = 0 ; i< payloadLen; i++){
//				result[i] = udp_msg->getPayload(i)^previous_msg->getPayload(i);
//			}

			// print the two packets for diagnostics
//			std::cout << "Pacchetti ottenuti da generazione: "<<udp_msg->getGeneration() << std::endl;
//
//			std::cout << "pacchetto1" <<std::endl;
//			if (udp_msg->getIsCombination()){
//				for(int i = 0 ; i< payloadLen; i++){
//					std::cout <<std::hex << previous_msg->getPayload(i) << " ";
//					//printf(" %x ", previous_msg->getPayload(i));
//
//				}
//			} else {
//				for(int i = 0 ; i< payloadLen; i++){
//					std::cout <<std::hex << udp_msg->getPayload(i) << " ";
//					//printf(" %x ", udp_msg->getPayload(i));
//
//				}
//			}
//			std::cout << std::endl << "pacchetto2" << std::endl;
//
//			for(int i = 0 ; i< payloadLen; i++){
//
//				std::cout <<std::hex << result[i]<< " ";
//				//printf(" %x ", result[i]);
//			}
//			std::cout << std::endl;
//
//			delete [] result;
//
//		}
//
//
//		// get worst case
//		simtime_t start_time = (udp_msg->getCreationTime() < previous_msg->getCreationTime()) ? udp_msg->getCreationTime() : previous_msg->getCreationTime();
//		// send to the statistic collector
//		std::cout << " # "<<getFullPath()<<" Salvo " << fabs(simTime() - start_time) << std::endl;
//		check_and_cast<UDPAppNC *>(getParentModule()->getParentModule()->getModuleByRelativePath(par("statistics_collector").stringValue()))->saveStatistics(udp_msg->getGeneration(),fabs(simTime() - start_time));
//
//
//		delete msg;
//		delete previous_msg;
//		nc_table.erase(it);
//
//
//	}
//}

