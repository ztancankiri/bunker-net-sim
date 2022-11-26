#ifndef __INET_HEARTBEATAPP_H
#define __INET_HEARTBEATAPP_H

#include "UdpBasicAppX.h"

namespace inet {

class INET_API HeartBeatApp : public UdpBasicAppX
{
    protected:
        typedef UdpBasicAppX super;
    protected:
        void sendPacket() override;
};

} // namespace inet

#endif
