#ifndef UDPSERVERAPP_H_
#define UDPSERVERAPP_H_

#include <unordered_map>
#include "UdpBasicAppX.h"
#include "inet/networklayer/common/L3AddressTag_m.h"

namespace inet {

class INET_API UdpServerApp : public UdpBasicAppX {
    protected:
        typedef UdpBasicAppX super;

        struct SurvivorData {
            L3Address ip;
            int bunkerId;
            simtime_t ts;
        };
        std::unordered_map<std::string, SurvivorData> survivorDatabase;

    protected:
        void initialize(int stage) override;
        void socketDataArrived(UdpSocket *socket, Packet *packet) override;
        void finish() override;

    protected:
        int successfulLookupCount = 0;
        int unsuccessfulLookupCount = 0;
        simsignal_t successfulLookup;
        simsignal_t unsuccessfulLookup;
};


} /* namespace inet */

#endif /* UDPSERVERAPP_H_ */
