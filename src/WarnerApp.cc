#include "WarnerApp.h"

#include "inet/applications/base/ApplicationPacket_m.h"
#include "inet/common/ModuleAccess.h"
#include "inet/common/TagBase_m.h"
#include "inet/common/TimeTag_m.h"
#include "inet/common/lifecycle/ModuleOperations.h"
#include "inet/common/packet/Packet.h"
#include "inet/networklayer/common/FragmentationTag_m.h"
#include "inet/networklayer/common/L3AddressResolver.h"
#include "inet/transportlayer/contract/udp/UdpControlInfo_m.h"
#include "BunkerPacket_m.h"

namespace inet {

Define_Module(WarnerApp);

WarnerApp::~WarnerApp()
{
    cancelAndDelete(selfMsg);
}

void WarnerApp::initialize(int stage)
{
    ClockUserModuleMixin::initialize(stage);

    if (stage == INITSTAGE_LOCAL) {
        destPort = par("destPort");
        selfMsg = new ClockEvent("sendTimer");
        serverAddress = L3AddressResolver().resolve(par("serverAddress"));
        serverPort = par("serverPort");
    }

    if (stage == INITSTAGE_APPLICATION_LAYER) {
        mecAppId = par("mecAppId");
        requiredRam = par("requiredRam").doubleValue();
        requiredDisk = par("requiredDisk").doubleValue();
        requiredCpu = par("requiredCpu").doubleValue();

        const char *mp1Ip = par("mp1Address");
        mp1Address = L3AddressResolver().resolve(mp1Ip);
        mp1Port = par("mp1Port");
    }
}

void WarnerApp::finish()
{
    ApplicationBase::finish();
}

void WarnerApp::setSocketOptions()
{
    int localPort = par("localUePort");

    socket.setOutputGate(gate("socketOut"));
    socket.bind(localPort);
    socket.setBroadcast(true);
    socket.setCallback(this);
}

void WarnerApp::sendPacket()
{
    Packet *packet = new Packet("Warning Message");
    packet->addTag<FragmentationReq>()->setDontFragment(true);

    const auto& payload = makeShared<BunkerPacket>();
    payload->setChunkLength(B(20));
    payload->setType(54);

    payload->addTag<CreationTimeTag>()->setCreationTime(simTime());
    packet->insertAtBack(payload);

    L3Address destAddr = L3Address("10.0.2.255"); // Broadcast address
    socket.sendTo(packet, destAddr, destPort);

    EV_INFO << getFullName() << " Warning Sent!" << endl;
}

void WarnerApp::processStart()
{
    setSocketOptions();

    selfMsg->setKind(SEND);
    processSend();
}

void WarnerApp::processSend()
{
    sendPacket();

    clocktime_t delay = par("sendInterval");
    selfMsg->setKind(SEND);
    scheduleClockEventAfter(delay, selfMsg);
}

void WarnerApp::handleMessageWhenUp(cMessage *msg)
{
    if (msg->isSelfMessage()) {
        ASSERT(msg == selfMsg);

        switch (selfMsg->getKind()) {
            case START:
                processStart();
                break;

            case SEND:
                processSend();
                break;

            default:
                throw cRuntimeError("Invalid kind %d in self message", (int)selfMsg->getKind());
        }
    }
    else
        socket.processMessage(msg);
}

void WarnerApp::socketDataArrived(UdpSocket *socket, Packet *packet)
{
    EV_INFO << "Received packet from Warner" << endl;

    L3Address remoteAddress = packet->getTag<L3AddressInd>()->getSrcAddress();
    int srcPort = packet->getTag<L4PortInd>()->getSrcPort();
    packet->clearTags();
    packet->trim();

    auto data = packet->peekData<BunkerPacket>();
    auto packetType = data->getType();

    if (packetType == 4) { // Warning Trigger Message
        warningMessage = std::string(data->getTextMessage());

        const auto& payload = makeShared<BunkerPacket>();
        payload->setChunkLength(B(20));
        payload->setType(5);  // Warning Lookup Request

        const auto& payload = makeShared<BunkerPacket>();
        payload->setChunkLength(B(20));
        payload->setBunkerId(data->getBunkerId());

        payload->addTag<CreationTimeTag>()->setCreationTime(simTime());
        packet->insertAtBack(payload);

        emit(packetSentSignal, packet);
        socket.sendTo(packet, serverAddress, serverPort);
        numSent++;
    }
    else if(packetType == 5) { // Warning Lookup request
        auto ip_list = data->getWarningIPs();

        char *ip_str = strtok(ip_list, "|");

        while (ip_str != NULL) {
            L3Address ip(ip_str);

            EV_INFO << "WARNING MESSAGE SENDING: WARNER APP " << "---->" << ip_str << endl;
            Packet *packet = new Packet("Warning Message");
            packet->addTag<FragmentationReq>()->setDontFragment(true);

            const auto& payload = makeShared<BunkerPacket>();

            payload->setChunkLength(B(20));
            payload->setType(7);
            payload->setTextMessage(warningMessage);

            payload->addTag<CreationTimeTag>()->setCreationTime(simTime());
            packet->insertAtBack(payload);

            emit(packetSentSignal, packet);
            socket.sendTo(packet, ip, 5555);
            numSent++;

            ip_str = strtok(NULL, "|");
        }
    }

    delete packet;
}

void WarnerApp::socketErrorArrived(UdpSocket *socket, Indication *indication)
{
    EV_WARN << "Ignoring UDP error report " << indication->getName() << endl;
    delete indication;
}

void WarnerApp::socketClosed(UdpSocket *socket)
{
    if (operationalState == State::STOPPING_OPERATION)
        startActiveOperationExtraTimeOrFinish(par("stopOperationExtraTime"));
}

void WarnerApp::handleStartOperation(LifecycleOperation *operation)
{
    selfMsg->setKind(START);
    scheduleClockEventAt(getClockTime(), selfMsg);
}

void WarnerApp::handleStopOperation(LifecycleOperation *operation)
{
    cancelEvent(selfMsg);
    socket.close();
    delayActiveOperationFinish(par("stopOperationTimeout"));
}

void WarnerApp::handleCrashOperation(LifecycleOperation *operation)
{
    cancelClockEvent(selfMsg);
    socket.destroy();
}

}
