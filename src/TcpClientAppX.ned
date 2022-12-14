package bunker_net_sim;

import inet.applications.contract.IApp;

simple TcpClientAppX like IApp
{
    parameters:        
        int listenPort = default(1000);
        int localPort = default(-1);
        string localAddress = default(""); // may be left empty ("")
        
        int bunker_number;    
        int host_number;
        int nonExist_host_number;
        string host_prefix = default("bunkerhost");
        
        string clockModule = default(""); // relative path of a module that implements IClock; optional
        
        bool sendingEnabled = default(true);
        
        @lifecycleSupport;
        double stopOperationExtraTime @unit(s) = default(-1s);
        double stopOperationTimeout @unit(s) = default(2s);  
        
        double sendInterval @unit(s) = default(5s);
        string connectAddress = default("");  // server address (may be symbolic)
        int connectPort = default(1000);
        double startTime @unit(s) = default(1s); // time first session begins
        double stopTime @unit(s) = default(-1s);  // time of finishing sending, negative values mean forever
        volatile double reconnectInterval @unit(s) = default(30s);
        
        @signal[chunkLength](type="long");
        @statistic[chunkLength](title="Chunk Length"; source="chunkLength"; record=vector; interpolationmode=none);
        
        @signal[endtoenddelay](type="long");
        @statistic[endtoenddelay](title="End-to-end Delay"; source="endtoenddelay"; record=vector; interpolationmode=linear);
        
        @signal[packetSent](type=inet::Packet);
        @signal[packetReceived](type=inet::Packet);
        @statistic[packetReceived](title="packets received"; source=packetReceived; record=count,"sum(packetBytes)","vector(packetBytes)"; interpolationmode=none);
        @statistic[throughput](title="throughput"; unit=bps; source="throughput(packetSent)"; record=vector);
        @statistic[packetSent](title="packets sent"; source=packetSent; record=count,"sum(packetBytes)","vector(packetBytes)"; interpolationmode=none);
        @statistic[rcvdPkLifetime](title="received packet lifetime"; source="dataAge(packetReceived)"; unit=s; record=stats,vector; interpolationmode=none);
        @statistic[rcvdPkSeqNo](title="received packet sequence number"; source="appPkSeqNo(packetReceived)"; record=vector; interpolationmode=none);
    gates:
        input socketIn @labels(TcpCommand/up);
        output socketOut @labels(TcpCommand/down);
}

simple TcpClientThread
{
}