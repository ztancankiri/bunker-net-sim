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

#include "Server.h"

#include "inet/common/ModuleAccess.h"
#include "inet/common/Simsignals.h"
#include "inet/transportlayer/common/L4PortTag_m.h"
#include "inet/transportlayer/contract/udp/UdpControlInfo_m.h"
#include "inet/networklayer/common/FragmentationTag_m.h"
#include "inet/applications/base/ApplicationPacket_m.h"
#include "inet/common/TimeTag_m.h"
#include "BunkerPacket_m.h"

namespace inet {

Define_Module(Server);

void Server::initialize(int stage)
{
    super::initialize(stage);

    if (stage == INITSTAGE_LOCAL) {
        numSent = 0;
        WATCH(numSent);
    }
}

void Server::socketDataArrived(UdpSocket *socket, Packet *pk)
{
    // determine its source address/port
    L3Address remoteAddress = pk->getTag<L3AddressInd>()->getSrcAddress();
    int srcPort = pk->getTag<L4PortInd>()->getSrcPort();
    pk->clearTags();
    pk->trim();
    auto data = pk->peekData<BunkerPacket>();
    EV_INFO <<"---------------------" << data->getSurvivorName() << "---------------------" <<endl;
    survivorDatabase["Erdem"].bunkerId = 2;


    const char *resBody = par("resBody");
    Packet *packet = new Packet(resBody);
    packet->addTag<FragmentationReq>()->setDontFragment(true);

    const auto& payload = makeShared<BunkerPacket>();
    int seqNum = 5454;
    payload->setChunkLength(B(20));
    payload->setSurvivorName("Erdem");

//    payload->setChunkLength(B(par("messageLength")));
//    payload->setSequenceNumber(connected_map["asd"]);
//    payload->addTag<CreationTimeTag>()->setCreationTime(simTime());

    packet->insertAtBack(payload);

    emit(packetSentSignal, packet);
    socket->sendTo(packet, remoteAddress, srcPort);
    numSent++;

}

} /* namespace inet */
