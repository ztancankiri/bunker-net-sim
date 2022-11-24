//
// Copyright (C) 2000 Institut fuer Telematik, Universitaet Karlsruhe
// Copyright (C) 2004,2011 OpenSim Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later
//


#ifndef __INET_CLIENT_H
#define __INET_CLIENT_H


#include "inet/applications/udpapp/UdpBasicApp.h"

namespace inet {

class INET_API Client : public UdpBasicApp
{
    protected:
        void sendPacket() override;
};

} // namespace inet

#endif

