#ifndef BASICAPPX_H_
#define BASICAPPX_H_

#include <vector>

#include "inet/applications/base/ApplicationBase.h"
#include "inet/common/clock/ClockUserModuleMixin.h"
#include "inet/transportlayer/contract/udp/UdpSocket.h"
#include "inet/common/socket/SocketMap.h"
#include "inet/transportlayer/contract/tcp/TcpSocket.h"

namespace inet {

// forward declaration:
class TcpServerThreadBaseX;

extern template class ClockUserModuleMixin<ApplicationBase>;

class INET_API BasicAppX : public ClockUserModuleMixin<ApplicationBase>, public UdpSocket::ICallback, public TcpSocket::ICallback
{
  protected:
    enum SelfMsgKinds { START = 1, SEND, STOP };
    clocktime_t startTime;
    clocktime_t stopTime;

    std::string ownName;

    const char *packetName = nullptr;

    bool sendingEnabled;
    L3Address destAddress;
    int destPort;

    bool listeningEnabled;
    int localPort;

    UdpSocket socket;
    ClockEvent *selfMsg = nullptr;
  protected:
    virtual int numInitStages() const override { return NUM_INIT_STAGES; }//same
    virtual void initialize(int stage) override;//same
    virtual void handleMessageWhenUp(cMessage *msg) override;//same
    virtual void finish() override;//same
    virtual void refreshDisplay() const override;//same

    virtual void sendPacket();//udp

    virtual void processStart();//udp
    virtual void processSend();//udp
    virtual void processStop();//udp

    virtual void handleStartOperation(LifecycleOperation *operation) override;//same
    virtual void handleStopOperation(LifecycleOperation *operation) override;//same
    virtual void handleCrashOperation(LifecycleOperation *operation) override;//same

    virtual void socketDataArrived(UdpSocket *socket, Packet *packet) override;//udp
    virtual void socketErrorArrived(UdpSocket *socket, Indication *indication) override;//udp
    virtual void socketClosed(UdpSocket *socket) override;//udp

    //TCP
    virtual void socketDataArrived(TcpSocket *socket, Packet *packet, bool urgent) override { throw cRuntimeError("Unexpected data"); }
    virtual void socketAvailable(TcpSocket *socket, TcpAvailableInfo *availableInfo) override;
    virtual void socketEstablished(TcpSocket *socket) override {}
    virtual void socketPeerClosed(TcpSocket *socket) override {}
    virtual void socketClosed(TcpSocket *socket) override;
    virtual void socketFailure(TcpSocket *socket, int code) override {}
    virtual void socketStatusArrived(TcpSocket *socket, TcpStatusInfo *status) override {}
    virtual void socketDeleted(TcpSocket *socket) override { socketMap.removeSocket(socket); }
  public:
  //virtual ~BasicAppX() { socketMap.deleteSockets(); }
    virtual void removeThread(TcpServerThreadBaseX *thread);
    virtual void threadClosed(TcpServerThreadBaseX *thread);
    friend class TcpServerThreadBaseX;

  protected:
    TcpSocket serverSocket;
    SocketMap socketMap;
    typedef std::set<TcpServerThreadBaseX *> ThreadSet;
    ThreadSet threadSet;

  public:
    BasicAppX() {}
    ~BasicAppX();
};

/**
 * Abstract base class for server processes to be used with BasicAppX.
 * Subclasses need to be registered using the Register_Class() macro.
 *
 * @see BasicAppX
 */
class INET_API TcpServerThreadBaseX : public cSimpleModule, public TcpSocket::ICallback
{
  protected:
    BasicAppX *hostmod;
    TcpSocket *sock; // ptr into socketMap managed by BasicAppX

    // internal: TcpSocket::ICallback methods
    virtual void socketDataArrived(TcpSocket *socket, Packet *msg, bool urgent) override { dataArrived(msg, urgent); }
    virtual void socketAvailable(TcpSocket *socket, TcpAvailableInfo *availableInfo) override { socket->accept(availableInfo->getNewSocketId()); }
    virtual void socketEstablished(TcpSocket *socket) override { established(); }
    virtual void socketPeerClosed(TcpSocket *socket) override { peerClosed(); }
    virtual void socketClosed(TcpSocket *socket) override { hostmod->threadClosed(this); }
    virtual void socketFailure(TcpSocket *socket, int code) override { failure(code); }
    virtual void socketStatusArrived(TcpSocket *socket, TcpStatusInfo *status) override { statusArrived(status); }
    virtual void socketDeleted(TcpSocket *socket) override;

    virtual void refreshDisplay() const override;

  public:

    TcpServerThreadBaseX() { sock = nullptr; hostmod = nullptr; }
    virtual ~TcpServerThreadBaseX() { delete sock; }

    // internal: called by BasicAppX after creating this module
    virtual void init(BasicAppX *hostmodule, TcpSocket *socket) { hostmod = hostmodule; sock = socket; }

    /*
     * Returns the socket object
     */
    virtual TcpSocket *getSocket() { return sock; }

    /*
     * Returns pointer to the host module
     */
    virtual BasicAppX *getHostModule() { return hostmod; }

    /**
     * Called when connection is established. To be redefined.
     */
    virtual void established() = 0;

    /*
     * Called when a data packet arrives. To be redefined.
     */
    virtual void dataArrived(Packet *msg, bool urgent) = 0;

    /*
     * Called when a timer (scheduled via scheduleAt()) expires. To be redefined.
     */
    virtual void timerExpired(cMessage *timer) = 0;

    /*
     * Called when the client closes the connection. By default it closes
     * our side too, but it can be redefined to do something different.
     */
    virtual void peerClosed() { getSocket()->close(); }

    /*
     * Called when the connection breaks (TCP error). By default it deletes
     * this thread, but it can be redefined to do something different.
     */
    virtual void failure(int code) { hostmod->removeThread(this); }

    /*
     * Called when a status arrives in response to getSocket()->getStatus().
     * By default it deletes the status object, redefine it to add code
     * to examine the status.
     */
    virtual void statusArrived(TcpStatusInfo *status) {}
};

} // namespace inet

#endif /* BASICAPPX_H_ */
