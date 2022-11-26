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
        const char *possibleSurvivorsStr = par("possibleSurvivors");
        cStringTokenizer tokenizer(possibleSurvivorsStr);
        const char *token;

        while ((token = tokenizer.nextToken()) != nullptr) {
            possibleSurvivors.push_back(token);

            if (textingSelectionAdded.find(token) == textingSelectionAdded.end()) {
                textingSelection.push_back(token);
                textingSelectionAdded.insert(token);
            }
        }
    }
}

std::string ClientApp::randomSelectForLookup() {
    std::string survivor = "";

    do {
        if (possibleSurvivors.size() == addressBook.size()) {
            survivor = "";
            break;
        }

        int k = intrand(possibleSurvivors.size());
        survivor = possibleSurvivors[k];
    } while (addressBook.find(survivor) != addressBook.end());

    if (survivor.size() > 0) {
        EV_INFO << "--- CLIENT: LOOKUP SELECTED: " << survivor << endl;
    }
    else {
        EV_INFO << "--- CLIENT: LOOKUP COULD NOT BE SELECTED" << endl;
    }

    return survivor;
}

std::string ClientApp::randomSelectForTexting() {
    std::string survivor = "";
    int size = textingSelection.size();

    if (size > 0) {
        int k = intrand(size);
        survivor = textingSelection[k];
    }

    if (survivor.size() > 0) {
        EV_INFO << "--- CLIENT: TEXT RECEIVER SELECTED: " << survivor << endl;
    }
    else {
        EV_INFO << "--- CLIENT: TEXT RECEIVER COULD NOT BE SELECTED" << endl;
    }

    return survivor;
}

void ClientApp::sendLookupRequest() {
    std::string survivor = randomSelectForLookup();

    if (survivor.size() > 0) {
        sendLookupRequest(survivor);
    }
}

void ClientApp::sendLookupRequest(std::string survivor) {
    Packet *packet = new Packet("Lookup Request");
    packet->addTag<FragmentationReq>()->setDontFragment(true);

    const auto& payload = makeShared<BunkerPacket>();
    payload->setChunkLength(B(20));
    payload->setType(1);

    payload->setSurvivorName(survivor.c_str());

    payload->addTag<CreationTimeTag>()->setCreationTime(simTime());
    packet->insertAtBack(payload);

    emit(packetSentSignal, packet);
    socket.sendTo(packet, destAddress, destPort);
}

void ClientApp::sendTextMessage() {
    std::string survivor = randomSelectForTexting();

    if (survivor.size() > 0) {
        if (addressBook.find(survivor) != addressBook.end()) { // Survivor exists in AdressBook
            sendTextMessage(survivor);
        }
        else {  // Survivor does not exist in AdressBook
            sendLookupRequest(survivor);
        }
    }
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
    int k = intrand(100);

    if (k < 50) {
        sendLookupRequest();
    }
    else {
        sendTextMessage();
    }
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

            if (textingSelectionAdded.find(survivorName) == textingSelectionAdded.end()) {
                textingSelection.push_back(survivorName);
                textingSelectionAdded.insert(survivorName);
            }

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

        if (textingSelectionAdded.find(survivorName) == textingSelectionAdded.end()) {
            textingSelection.push_back(survivorName);
            textingSelectionAdded.insert(survivorName);
        }

        EV_INFO << "--- CLIENT: TEXT MESSAGE RECEIVED: " << textMessage << endl;
    }

    delete pk;
}

} // namespace inet
