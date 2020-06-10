simple Host
{
    parameters:
        @display("i=device/pc");
        double xpos;
        double ypos;
        @display("p=$xpos,$ypos");
    gates:
        input in;
        output out;
}

simple Switch
{
    parameters:
        double xpos;
        double ypos;
        @display("p=$xpos,$ypos");
        @display("i=device/switch");
    gates:
        input in[];
        output out[];    
}

network FatTree
{
    parameters:
        @display("bgb=1500");
        switchs[0].xpos = 200;
        switchs[0].ypos = 0;
        switchs[1].xpos = 500;
        switchs[1].ypos = 0;
        switchs[2].xpos = 900;
        switchs[2].ypos = 0;
        switchs[3].xpos = 1200;
        switchs[3].ypos = 0;
        
        switchs[4].xpos = 0;
        switchs[4].ypos = 200;
        switchs[5].xpos = 200;
        switchs[5].ypos = 200;
        switchs[6].xpos = 400;
        switchs[6].ypos = 200;
        switchs[7].xpos = 600;
        switchs[7].ypos = 200;
        switchs[8].xpos = 800;
        switchs[8].ypos = 200;
        switchs[9].xpos = 1000;
        switchs[9].ypos = 200;
        switchs[10].xpos = 1200;
        switchs[10].ypos = 200;
        switchs[11].xpos = 1400;
        switchs[11].ypos = 200;
        
        switchs[12].xpos = 0;
        switchs[12].ypos = 400;
        switchs[13].xpos = 200;
        switchs[13].ypos = 400;
        switchs[14].xpos = 400;
        switchs[14].ypos = 400;
        switchs[15].xpos = 600;
        switchs[15].ypos = 400;
        switchs[16].xpos = 800;
        switchs[16].ypos = 400;
        switchs[17].xpos = 1000;
        switchs[17].ypos = 400;
        switchs[18].xpos = 1200;
        switchs[18].ypos = 400;
        switchs[19].xpos = 1400;
        switchs[19].ypos = 400;
        
        hosts[0].xpos = 0;
        hosts[0].ypos = 600;
        hosts[1].xpos = 100;
        hosts[1].ypos = 600;
        hosts[2].xpos = 200;
        hosts[2].ypos = 600;
        hosts[3].xpos = 300;
        hosts[3].ypos = 600;
        hosts[4].xpos = 400;
        hosts[4].ypos = 600;
        hosts[5].xpos = 500;
        hosts[5].ypos = 600;
        hosts[6].xpos = 600;
        hosts[6].ypos = 600;
        hosts[7].xpos = 700;
        hosts[7].ypos = 600;
        hosts[8].xpos = 800;
        hosts[8].ypos = 600;
        hosts[9].xpos = 900;
        hosts[9].ypos = 600;
        hosts[10].xpos = 1000;
        hosts[10].ypos = 600;
        hosts[11].xpos = 1100;
        hosts[11].ypos = 600;
        hosts[12].xpos = 1200;
        hosts[12].ypos = 600;
        hosts[13].xpos = 1300;
        hosts[13].ypos = 600;
        hosts[14].xpos = 1400;
        hosts[14].ypos = 600;
        hosts[15].xpos = 1500;
        hosts[15].ypos = 600;
    types:
        channel Channel extends ned.DelayChannel {
            delay = 100ms;
        }
    submodules:
        hosts[16]: Host;
        switchs[20]: Switch;
    connections:
        switchs[0].out++ --> Channel --> switchs[4].in++;
        switchs[0].in++ <-- Channel <-- switchs[4].out++;
        switchs[0].out++ --> Channel --> switchs[6].in++;
        switchs[0].in++ <-- Channel <-- switchs[6].out++;
        switchs[0].out++ --> Channel --> switchs[8].in++;
        switchs[0].in++ <-- Channel <-- switchs[8].out++;
        switchs[0].out++ --> Channel --> switchs[10].in++;
        switchs[0].in++ <-- Channel <-- switchs[10].out++;
        
        switchs[1].out++ --> Channel --> switchs[4].in++;
        switchs[1].in++ <-- Channel <-- switchs[4].out++;
        switchs[1].out++ --> Channel --> switchs[6].in++;
        switchs[1].in++ <-- Channel <-- switchs[6].out++;
        switchs[1].out++ --> Channel --> switchs[8].in++;
        switchs[1].in++ <-- Channel <-- switchs[8].out++;
        switchs[1].out++ --> Channel --> switchs[10].in++;
        switchs[1].in++ <-- Channel <-- switchs[10].out++;
        
        switchs[2].out++ --> Channel --> switchs[5].in++;
        switchs[2].in++ <-- Channel <-- switchs[5].out++;
        switchs[2].out++ --> Channel --> switchs[7].in++;
        switchs[2].in++ <-- Channel <-- switchs[7].out++;
        switchs[2].out++ --> Channel --> switchs[9].in++;
        switchs[2].in++ <-- Channel <-- switchs[9].out++;
        switchs[2].out++ --> Channel --> switchs[11].in++;
        switchs[2].in++ <-- Channel <-- switchs[11].out++;
        
        switchs[3].out++ --> Channel --> switchs[5].in++;
        switchs[3].in++ <-- Channel <-- switchs[5].out++;
        switchs[3].out++ --> Channel --> switchs[7].in++;
        switchs[3].in++ <-- Channel <-- switchs[7].out++;
        switchs[3].out++ --> Channel --> switchs[9].in++;
        switchs[3].in++ <-- Channel <-- switchs[9].out++;
        switchs[3].out++ --> Channel --> switchs[11].in++;
        switchs[3].in++ <-- Channel <-- switchs[11].out++;
        
        switchs[4].out++ --> Channel --> switchs[12].in++;
        switchs[4].in++ <-- Channel <-- switchs[12].out++;
        switchs[4].out++ --> Channel --> switchs[13].in++;
        switchs[4].in++ <-- Channel <-- switchs[13].out++;
        
        switchs[5].out++ --> Channel --> switchs[12].in++;
        switchs[5].in++ <-- Channel <-- switchs[12].out++;
        switchs[5].out++ --> Channel --> switchs[13].in++;
        switchs[5].in++ <-- Channel <-- switchs[13].out++;
        
        switchs[6].out++ --> Channel --> switchs[14].in++;
        switchs[6].in++ <-- Channel <-- switchs[14].out++;
        switchs[6].out++ --> Channel --> switchs[15].in++;
        switchs[6].in++ <-- Channel <-- switchs[15].out++;
        
        switchs[7].out++ --> Channel --> switchs[14].in++;
        switchs[7].in++ <-- Channel <-- switchs[14].out++;
        switchs[7].out++ --> Channel --> switchs[15].in++;
        switchs[7].in++ <-- Channel <-- switchs[15].out++;
        
        switchs[8].out++ --> Channel --> switchs[16].in++;
        switchs[8].in++ <-- Channel <-- switchs[16].out++;
        switchs[8].out++ --> Channel --> switchs[17].in++;
        switchs[8].in++ <-- Channel <-- switchs[17].out++;
        
        switchs[9].out++ --> Channel --> switchs[16].in++;
        switchs[9].in++ <-- Channel <-- switchs[16].out++;
        switchs[9].out++ --> Channel --> switchs[17].in++;
        switchs[9].in++ <-- Channel <-- switchs[17].out++;
        
        switchs[10].out++ --> Channel --> switchs[18].in++;
        switchs[10].in++ <-- Channel <-- switchs[18].out++;
        switchs[10].out++ --> Channel --> switchs[19].in++;
        switchs[10].in++ <-- Channel <-- switchs[19].out++;
        
        switchs[11].out++ --> Channel --> switchs[18].in++;
        switchs[11].in++ <-- Channel <-- switchs[18].out++;
        switchs[11].out++ --> Channel --> switchs[19].in++;
        switchs[11].in++ <-- Channel <-- switchs[19].out++;
        
        hosts[0].out --> Channel --> switchs[12].in++;
        hosts[0].in <-- Channel <-- switchs[12].out++;
        
        hosts[1].out --> Channel --> switchs[12].in++;
        hosts[1].in <-- Channel <-- switchs[12].out++;
        
        hosts[2].out --> Channel --> switchs[13].in++;
        hosts[2].in <-- Channel <-- switchs[13].out++;
        
        hosts[3].out --> Channel --> switchs[13].in++;
        hosts[3].in <-- Channel <-- switchs[13].out++;
        
        hosts[4].out --> Channel --> switchs[14].in++;
        hosts[4].in <-- Channel <-- switchs[14].out++;
        
        hosts[5].out --> Channel --> switchs[14].in++;
        hosts[5].in <-- Channel <-- switchs[14].out++;
        
        hosts[6].out --> Channel --> switchs[15].in++;
        hosts[6].in <-- Channel <-- switchs[15].out++;
        
        hosts[7].out --> Channel --> switchs[15].in++;
        hosts[7].in <-- Channel <-- switchs[15].out++;
        
        hosts[8].out --> Channel --> switchs[16].in++;
        hosts[8].in <-- Channel <-- switchs[16].out++;
        
        hosts[9].out --> Channel --> switchs[16].in++;
        hosts[9].in <-- Channel <-- switchs[16].out++;
        
        hosts[10].out --> Channel --> switchs[17].in++;
        hosts[10].in <-- Channel <-- switchs[17].out++;
        
        hosts[11].out --> Channel --> switchs[17].in++;
        hosts[11].in <-- Channel <-- switchs[17].out++;
        
        hosts[12].out --> Channel --> switchs[18].in++;
        hosts[12].in <-- Channel <-- switchs[18].out++;
        
        hosts[13].out --> Channel --> switchs[18].in++;
        hosts[13].in <-- Channel <-- switchs[18].out++;
        
        hosts[14].out --> Channel --> switchs[19].in++;
        hosts[14].in <-- Channel <-- switchs[19].out++;
        
        hosts[15].out --> Channel --> switchs[19].in++;
        hosts[15].in <-- Channel <-- switchs[19].out++;
}

