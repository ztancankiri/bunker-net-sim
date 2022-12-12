#ifndef __INET_TCPHEARTBEATAPP_H
#define __INET_TCPHEARTBEATAPP_H

#include "BasicAppX.h"

namespace inet {

class INET_API TcpHeartBeatApp : public BasicAppX
{
    protected:
        typedef BasicAppX super;
    protected:
        void sendPacket() override;
        void finish() override;
};

} // namespace inet

#endif
