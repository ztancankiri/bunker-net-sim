#ifndef __INET_HEARTBEATAPP_H
#define __INET_HEARTBEATAPP_H

#include "inet/applications/udpapp/UdpBasicApp.h"

namespace inet {

class INET_API HeartBeatApp : public UdpBasicApp
{
    protected:
        typedef UdpBasicApp super;
    protected:
        void sendPacket() override;
        void initialize(int stage) override;
        void socketDataArrived(UdpSocket *socket, Packet *packet) override;
};

} // namespace inet

#endif
