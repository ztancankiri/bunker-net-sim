package bunker_net_sim;

import inet.applications.contract.IApp;

simple TcpServerAppX like IApp
{
    parameters:        
        int heartbeatThreshold;
        int listenPort = default(1000);
        int localPort = default(-1);
        string localAddress = default(""); // may be left empty ("")
        
        
        @lifecycleSupport;
        double stopOperationExtraTime @unit(s) = default(-1s);
        double stopOperationTimeout @unit(s) = default(2s);  
        
        string connectAddress = default("");  // server address (may be symbolic)
        int connectPort = default(1000);
        double startTime @unit(s) = default(1s); // time first session begins
        double stopTime @unit(s) = default(-1s);  // time of finishing sending, negative values mean forever
        volatile double reconnectInterval @unit(s) = default(30s);
    gates:
        input socketIn @labels(TcpCommand/up);
        output socketOut @labels(TcpCommand/down);
}

simple TcpThread
{
}