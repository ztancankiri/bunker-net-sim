#ifndef SERVERAPP_H_
#define SERVERAPP_H_

#include <unordered_map>
#include "inet/applications/udpapp/UdpBasicApp.h"
#include "inet/networklayer/common/L3AddressTag_m.h"

namespace inet {

class INET_API ServerApp : public UdpBasicApp {
    protected:
        typedef UdpBasicApp super;

        struct SurvivorData {
            L3Address ip;
            int bunkerId;
            simtime_t ts;
        };
        std::unordered_map<std::string, SurvivorData> survivorDatabase;

    protected:
        void initialize(int stage) override;
        void socketDataArrived(UdpSocket *socket, Packet *packet) override;

        void handleStartOperation(LifecycleOperation *operation) override;
        void handleStopOperation(LifecycleOperation *operation) override;
        void handleCrashOperation(LifecycleOperation *operation) override;
};


} /* namespace inet */

#endif /* SERVERAPP_H_ */
