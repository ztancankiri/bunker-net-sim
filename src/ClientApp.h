#ifndef __INET_CLIENTAPP_H
#define __INET_CLIENTAPP_H

#include <unordered_map>
#include "UdpBasicAppX.h"
#include "inet/networklayer/common/L3AddressTag_m.h"

namespace inet {

class INET_API ClientApp : public UdpBasicAppX
{
    protected:
        typedef UdpBasicAppX super;

        struct addressEntry {
            L3Address ip;
            simtime_t ts;
        };

        std::string ownName;
        std::unordered_map<std::string, addressEntry> addressBook;
        std::vector<std::string> possibleSurvivors;

        int learntFromMessages;
        std::unordered_set<std::string> textingSelectionAdded;
        std::vector<std::string> textingSelection;
    protected:
        std::string randomSelectForLookup();
        std::string randomSelectForTexting();

        void sendLookupRequest();
        void sendLookupRequest(std::string survivor);

        void sendTextMessage();
        void sendTextMessage(std::string receiver);

        void sendPacket() override;
        void initialize(int stage) override;
        void socketDataArrived(UdpSocket *socket, Packet *packet) override;
};

} // namespace inet

#endif
