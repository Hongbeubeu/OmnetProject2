simple RingHost
{
    parameters:
        int numHosts;
    gates:
        input in;
        output out;
}

network ringhost
{
    parameters:
        int numHosts;
    submodules:
        hosts[numHosts]: RingHost;
    connections:
        hosts[numHosts-1].out --> {delay=1000ms;} -->hosts[0].in;
        for i=0..numHosts-2{
            hosts[i].out --> {delay=1000ms;} -->hosts[i+1].in;
        }
}
    