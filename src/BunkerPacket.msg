//
// Copyright (C) 2016 OpenSim Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later
//

import inet.common.INETDefs;
import inet.common.packet.chunk.Chunk;
import inet.networklayer.common.L3AddressTag;

namespace inet;

//
// Generic application packet
//
// TODO rename to ApplicationHeader and remove ByteCountChunk from ApplicationHeaderSerializer
class BunkerPacket extends FieldsChunk
{
    // 0=heartbeat 1=lookup request 2=lookup response
    int type;
    string survivorName;
    L3Address ip;
    int bunkerId;
}
