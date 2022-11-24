//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#ifndef SERVER_H_
#define SERVER_H_

#include <unordered_map>
#include "inet/applications/udpapp/UdpEchoApp.h"

namespace inet {

class INET_API Server : public UdpEchoApp {
    protected:
        typedef UdpEchoApp super;

        int numSent = 0;
        std::unordered_map<std::string, int> connected_map;

    protected:
        void initialize(int stage) override;
        void socketDataArrived(UdpSocket *socket, Packet *packet) override;
};


} /* namespace inet */

#endif /* SERVER_H_ */
