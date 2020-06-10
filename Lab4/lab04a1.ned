simple SimpleHost
{
    parameters:
    	int limits;
    gates:
        input in;
        output out;
}

network sendLimited
{
    types:
        channel Channel extends ned.DelayChannel{
            delay = 1000ms;
        }
    submodules:
        sparrow: SimpleHost;
        eagle: SimpleHost;
    connections:
        sparrow.out --> Channel -->	eagle.in;
        sparrow.in <-- Channel <-- eagle.out;
}
        