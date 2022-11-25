#include "ClientApp.h"

#include "inet/applications/base/ApplicationPacket_m.h"
#include "inet/common/ModuleAccess.h"
#include "inet/common/TagBase_m.h"
#include "inet/common/TimeTag_m.h"
#include "inet/common/lifecycle/ModuleOperations.h"
#include "inet/common/packet/Packet.h"
#include "inet/networklayer/common/FragmentationTag_m.h"
#include "inet/networklayer/common/L3AddressResolver.h"
#include "inet/transportlayer/common/L4PortTag_m.h"
#include "inet/transportlayer/contract/udp/UdpControlInfo_m.h"
#include "BunkerPacket_m.h"

namespace inet {

Define_Module(ClientApp);

void ClientApp::initialize(int stage)
{
    super::initialize(stage);

    if (stage == INITSTAGE_LOCAL) {
        std::stringstream possibleSurvivorsStream(std::string(par("possibleSurvivors")));
        std::string tmp;

        while(getline(possibleSurvivorsStream, tmp, ' ')) {
            possibleSurvivors.push_back(tmp);
        }
    }
}

void ClientApp::sendLookupRequest()
{
    Packet *packet = new Packet("Lookup Request");
    packet->addTag<FragmentationReq>()->setDontFragment(true);

    const auto& payload = makeShared<BunkerPacket>();
    payload->setChunkLength(B(20));
    payload->setType(1);

    // Select a survivor
    std::string survivor = "";
    int tryCounter = 50;
    do {
        int k = intrand(possibleSurvivors.size());
        survivor = possibleSurvivors[k];
        tryCounter--;
    } while ((strcmp(survivor.c_str(), par("survivorName")) == 0 || addressBook.find(survivor) != addressBook.end()) && tryCounter > 0);
    EV_INFO << "--- CLIENT: SURVIVOR SELECTED: " << survivor << endl;

    payload->setSurvivorName(survivor.c_str());

    payload->addTag<CreationTimeTag>()->setCreationTime(simTime());
    packet->insertAtBack(payload);

    emit(packetSentSignal, packet);
    socket.sendTo(packet, chooseDestAddr(), destPort);
}

void ClientApp::sendTextMessage()
{

}

void ClientApp::sendTextMessage(std::string receiver)
{
    Packet *packet = new Packet("Text Message");
    packet->addTag<FragmentationReq>()->setDontFragment(true);

    const auto& payload = makeShared<BunkerPacket>();
    payload->setChunkLength(B(20));
    payload->setType(3);
    payload->setSurvivorName(par("survivorName"));
    payload->setTextMessage("Slava Ukraini !!!");


    payload->addTag<CreationTimeTag>()->setCreationTime(simTime());
    packet->insertAtBack(payload);

    emit(packetSentSignal, packet);
    socket.sendTo(packet, addressBook[receiver].ip, 5555);
}

void ClientApp::sendPacket()
{
    sendLookupRequest();
}

void ClientApp::socketDataArrived(UdpSocket *socket, Packet *pk)
{
    auto data = pk->peekData<BunkerPacket>();

    auto packetType = data->getType();
    auto survivorName = data->getSurvivorName();
    auto bunkerId = data->getBunkerId();
    auto ip = data->getIp();

    if (packetType == 2) {
        if (bunkerId == -1) { // Not Found
            EV_INFO << "--- CLIENT: SURVIVOR NOT FOUND: " << survivorName << endl;
        }
        else { // Survivor Found
            addressBook[survivorName].ip = ip;
            addressBook[survivorName].ts = simTime();
            EV_INFO << "--- CLIENT: SURVIVOR FOUND: " << survivorName << endl;
            sendTextMessage(survivorName);
        }
    }
    else if (packetType == 3) {
        L3Address remoteAddress = pk->getTag<L3AddressInd>()->getSrcAddress();
        auto textMessage = data->getTextMessage();

        // Add the sender to the address book.
        addressBook[survivorName].ip = remoteAddress;
        addressBook[survivorName].ts = simTime();

        EV_INFO << "--- CLIENT: TEXT MESSAGE RECEIVED: " << textMessage << endl;
    }
}

} // namespace inet
