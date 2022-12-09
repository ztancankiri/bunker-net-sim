#ifndef __INET_UDPHEARTBEATAPP_H
#define __INET_UDPHEARTBEATAPP_H

#include "UdpBasicAppX.h"

namespace inet {

class INET_API UdpHeartBeatApp : public UdpBasicAppX
{
    protected:
        typedef UdpBasicAppX super;
    protected:
        void sendPacket() override;
        void finish() override;
};

} // namespace inet

#endif
