import inet.common.INETDefs;
import inet.common.packet.chunk.Chunk;
import inet.networklayer.common.L3AddressTag;

namespace inet;

class BunkerPacket extends FieldsChunk
{
    // Type-0 = Heartbeat
    // Type-1 = Lookup request
    // Type-2 = Lookup response
    int type;
    string survivorName;
    L3Address ip;
    int bunkerId;
}