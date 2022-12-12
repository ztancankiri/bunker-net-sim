#include "BasicAppX.h"

#include "inet/applications/base/ApplicationPacket_m.h"
#include "inet/common/ModuleAccess.h"
#include "inet/common/TagBase_m.h"
#include "inet/common/TimeTag_m.h"
#include "inet/common/lifecycle/ModuleOperations.h"
#include "inet/common/packet/Packet.h"
#include "inet/networklayer/common/FragmentationTag_m.h"
#include "inet/networklayer/common/L3AddressResolver.h"
#include "inet/transportlayer/contract/udp/UdpControlInfo_m.h"
#include "inet/common/INETUtils.h"
#include "inet/common/stlutils.h"

namespace inet {

BasicAppX::~BasicAppX()
{
    //tcp
    socketMap.deleteSockets();
    //udp
    cancelAndDelete(selfMsg);
}

void BasicAppX::initialize(int stage)
{
    ClockUserModuleMixin::initialize(stage);

    if (stage == INITSTAGE_LOCAL) {
        ownName = std::string(getParentModule()->getFullName());
        startTime = par("startTime");
        stopTime = par("stopTime");

        sendingEnabled = par("sendingEnabled");
        listeningEnabled = par("listeningEnabled");

        if (listeningEnabled) {
            localPort = par("localPort");
        }

        if (stopTime >= CLOCKTIME_ZERO && stopTime < startTime) {
            throw cRuntimeError("Invalid startTime/stopTime parameters");
        }

        selfMsg = new ClockEvent("sendTimer");
    }

    if (stage == INITSTAGE_LAST) {
        if (sendingEnabled) {
            destAddress = L3AddressResolver().resolve(par("destAddress"));
            destPort = par("destPort");
        }
    }
}

void BasicAppX::finish()
{
    //TCP
    // remove and delete threads
    while (!threadSet.empty())
        removeThread(*threadSet.begin());

    //UDP
    ApplicationBase::finish();
}

void BasicAppX::sendPacket() {}

void BasicAppX::processStart()
{
    socket.setOutputGate(gate("socketOut"));
    if (listeningEnabled) {
        socket.bind(localPort);
    }

    socket.setCallback(this);

    if (sendingEnabled) {
        selfMsg->setKind(SEND);
        processSend();
    }
    else {
        if (stopTime >= CLOCKTIME_ZERO) {
            selfMsg->setKind(STOP);
            scheduleClockEventAt(stopTime, selfMsg);
        }
    }
}

void BasicAppX::processSend()
{
    sendPacket();
    clocktime_t d = par("sendInterval");
    if (stopTime < CLOCKTIME_ZERO || getClockTime() + d < stopTime) {
        selfMsg->setKind(SEND);
        scheduleClockEventAfter(d, selfMsg);
    }
    else {
        selfMsg->setKind(STOP);
        scheduleClockEventAt(stopTime, selfMsg);
    }
}

void BasicAppX::processStop()
{
    socket.close();
}

void BasicAppX::handleMessageWhenUp(cMessage *msg)
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

            case STOP:
                processStop();
                break;

            default:
                throw cRuntimeError("Invalid kind %d in self message", (int)selfMsg->getKind());
        }
    }
    else
        socket.processMessage(msg);

    //TCP

    if ((TcpServerThreadBaseX *)msg->getContextPointer() != nullptr){
        if (msg->isSelfMessage()) {
            TcpServerThreadBaseX *thread = (TcpServerThreadBaseX *)msg->getContextPointer();
            if (!contains(threadSet, thread))
                throw cRuntimeError("Invalid thread pointer in the timer (msg->contextPointer is invalid)");
            thread->timerExpired(msg);
        }
        else {
            TcpSocket *tcpsocket = check_and_cast_nullable<TcpSocket *>(socketMap.findSocketFor(msg));
            if (tcpsocket)
                tcpsocket->processMessage(msg);
            else if (serverSocket.belongsToSocket(msg))
                serverSocket.processMessage(msg);
            else {
    //            throw cRuntimeError("Unknown incoming message: '%s'", msg->getName());
                EV_ERROR << "message " << msg->getFullName() << "(" << msg->getClassName() << ") arrived for unknown socket \n";
                delete msg;
            }
        }
    }

}

void BasicAppX::socketDataArrived(UdpSocket *socket, Packet *packet) { }

void BasicAppX::socketErrorArrived(UdpSocket *socket, Indication *indication)
{
    EV_WARN << "Ignoring UDP error report " << indication->getName() << endl;
    delete indication;
}

void BasicAppX::socketClosed(UdpSocket *socket)
{
    if (operationalState == State::STOPPING_OPERATION)
        startActiveOperationExtraTimeOrFinish(par("stopOperationExtraTime"));
}

void BasicAppX::refreshDisplay() const
{
    ApplicationBase::refreshDisplay();

    char buf[100];
    sprintf(buf, "---");
    getDisplayString().setTagArg("t", 0, buf);
}

void BasicAppX::handleStartOperation(LifecycleOperation *operation)
{
    //TCP
    int tcplocalPort = par("tcplocalPort");

    serverSocket.setOutputGate(gate("socketOut"));
    serverSocket.setCallback(this);
    serverSocket.bind(tcplocalPort);
    serverSocket.listen();

    //UDP
    clocktime_t start = std::max(startTime, getClockTime());
    if ((stopTime < CLOCKTIME_ZERO) || (start < stopTime) || (start == stopTime && startTime == stopTime)) {
        selfMsg->setKind(START);
        scheduleClockEventAt(start, selfMsg);
    }
}

void BasicAppX::handleStopOperation(LifecycleOperation *operation)
{
    //tcp
    for (auto thread : threadSet)
        thread->getSocket()->close();
    serverSocket.close();

    //udp
    cancelEvent(selfMsg);
    socket.close();
    delayActiveOperationFinish(par("stopOperationTimeout"));
}

void BasicAppX::handleCrashOperation(LifecycleOperation *operation)
{
    //TCP
    // remove and delete threads
    while (!threadSet.empty()) {
        auto thread = *threadSet.begin();
        // TODO destroy!!!
        thread->getSocket()->close();
        removeThread(thread);
    }
    // TODO always?
    if (operation->getRootModule() != getContainingNode(this))
        serverSocket.destroy();

    //UDP
    cancelClockEvent(selfMsg);
    socket.destroy(); // TODO  in real operating systems, program crash detected by OS and OS closes sockets of crashed programs.
    socket.setCallback(nullptr);
}

//TCP
void BasicAppX::socketAvailable(TcpSocket *socket, TcpAvailableInfo *availableInfo)
{
    // new TCP connection -- create new socket object and server process
    TcpSocket *newSocket = new TcpSocket(availableInfo);
    newSocket->setOutputGate(gate("socketOut"));

    const char *serverThreadModuleType = par("serverThreadModuleType");
    cModuleType *moduleType = cModuleType::get(serverThreadModuleType);
    char name[80];
    sprintf(name, "thread_%i", newSocket->getSocketId());
    TcpServerThreadBaseX *proc = check_and_cast<TcpServerThreadBaseX *>(moduleType->create(name, this));
    proc->finalizeParameters();
    proc->callInitialize();

    newSocket->setCallback(proc);
    proc->init(this, newSocket);

    socketMap.addSocket(newSocket);
    threadSet.insert(proc);

    socket->accept(availableInfo->getNewSocketId());
}

void BasicAppX::socketClosed(TcpSocket *socket)
{
    if (operationalState == State::STOPPING_OPERATION && threadSet.empty() && !serverSocket.isOpen())
        startActiveOperationExtraTimeOrFinish(par("stopOperationExtraTime"));
}

void BasicAppX::removeThread(TcpServerThreadBaseX *thread)
{
    // remove socket
    socketMap.removeSocket(thread->getSocket());
    threadSet.erase(thread);

    // remove thread object
    thread->deleteModule();
}

void BasicAppX::threadClosed(TcpServerThreadBaseX *thread)
{
    // remove socket
    socketMap.removeSocket(thread->getSocket());
    threadSet.erase(thread);

    socketClosed(thread->getSocket());

    // remove thread object
    thread->deleteModule();
}

void TcpServerThreadBaseX::socketDeleted(TcpSocket *socket)
{
    if (socket == sock) {
        sock = nullptr;
        hostmod->socketDeleted(socket);
    }
}

void TcpServerThreadBaseX::refreshDisplay() const
{
    getDisplayString().setTagArg("t", 0, TcpSocket::stateName(sock->getState()));
}

} // namespace inet
