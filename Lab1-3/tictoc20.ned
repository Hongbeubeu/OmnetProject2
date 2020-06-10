
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

simple Txc20
{
    parameters:
        @display("i=block/routing");
        double xpos;
        double ypos;
        @display("p=$xpos,$ypos");
        
        int start;
        int finish;
    gates:
        input in[];  // declare in[] and out[] to be vector gates
        output out[];
}

network Tictoc20
{
    parameters:
        @display("bgb=600");
        node[0].xpos = 200;
        node[0].ypos = 100;
        node[1].xpos = 300;
        node[1].ypos = 100;
        node[2].xpos = 400;
        node[2].ypos = 100;
        node[3].xpos = 500;
        node[3].ypos = 200;
        node[4].xpos = 400;
        node[4].ypos = 400;
        node[5].xpos = 300;
        node[5].ypos = 400;
        node[6].xpos = 200;
        node[6].ypos = 400;
        node[7].xpos = 100;
        node[7].ypos = 400;
        node[8].xpos = 100;
        node[8].ypos = 200;
        node[9].xpos = 200;
        node[9].ypos = 200;
        node[10].xpos = 400;
        node[10].ypos = 200;
        node[11].xpos = 400;
        node[11].ypos = 300;
        node[12].xpos = 200;
        node[12].ypos = 300;
    types:
        channel Channel extends ned.DelayChannel {
            delay = 100ms;
        }
    submodules:
        node[13]: Txc20;
        
    connections:
        node[0].out++ --> Channel --> node[1].in++;
        node[0].in++ <-- Channel <-- node[1].out++;
        
        node[0].out++ --> Channel --> node[9].in++;
        node[0].in++ <-- Channel <-- node[9].out++;
        
        node[1].out++ --> Channel --> node[2].in++;
        node[1].in++ <-- Channel <-- node[2].out++;
        
        node[2].out++ --> Channel --> node[3].in++;
        node[2].in++ <-- Channel <-- node[3].out++;
        
        node[2].out++ --> Channel --> node[10].in++;
        node[2].in++ <-- Channel <-- node[10].out++;
        
        node[3].out++ --> Channel --> node[11].in++;
        node[3].in++ <-- Channel <-- node[11].out++;
        
        node[10].out++ --> Channel --> node[11].in++;
        node[10].in++ <-- Channel <-- node[11].out++;
        
        node[11].out++ --> Channel --> node[4].in++;
        node[11].in++ <-- Channel <-- node[4].out++;
        
        node[4].out++ --> Channel --> node[5].in++;
        node[4].in++ <-- Channel <-- node[5].out++;
        
        node[5].out++ --> Channel --> node[6].in++;
        node[5].in++ <-- Channel <-- node[6].out++;
        
        node[6].out++ --> Channel --> node[7].in++;
        node[6].in++ <-- Channel <-- node[7].out++;
        
        node[6].out++ --> Channel --> node[12].in++;
        node[6].in++ <-- Channel <-- node[12].out++;
        
        node[7].out++ --> Channel --> node[8].in++;
        node[7].in++ <-- Channel <-- node[8].out++;
        
        node[8].out++ --> Channel --> node[9].in++;
        node[8].in++ <-- Channel <-- node[9].out++;
        
        node[9].out++ --> Channel --> node[12].in++;
        node[9].in++ <-- Channel <-- node[12].out++;
}