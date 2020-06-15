simple Nodes
{
	parameters:
		@display("i=device/pc");
		int type;
		int des = default(-1);
		int EXB_SIZE;
		double TIMEOUT;
		double TIME_INTERVAL;
		double CHANNEL_DELAY;
		double CREDIT_DELAY;
		double TIME_GEN_MSG;
		double TIME_OPERATION_OF_SWITCH;
	gates:
		input in[4];
		output out[4];
}

network fattree
{
	parameters:
		Node[4].type = 2;
		Node[5].type = 2;
		Node[6].type = 2;
		Node[7].type = 2;
		Node[12].type = 2;
		Node[13].type = 2;
		Node[14].type = 2;
		Node[15].type = 2;
		Node[20].type = 2;
		Node[21].type = 2;
		Node[22].type = 2;
		Node[23].type = 2;
		Node[28].type = 2;
		Node[29].type = 2;
		Node[30].type = 2;
		Node[31].type = 2;
		Node[32].type = 2;
		Node[33].type = 2;
		Node[34].type = 2;
		Node[35].type = 2;
		Node[1].type = 1;
		Node[8].type = 3;
		Node[19].type = 1;
		Node[9].type = 3;
		Node[18].type = 1;
		Node[11].type = 3;
		Node[16].type = 1;
		Node[3].type = 3;
		Node[17].type = 1;
		Node[24].type = 3;
		Node[0].type = 1;
		Node[25].type = 3;
		Node[27].type = 1;
		Node[2].type = 3;
		Node[10].type = 1;
		Node[26].type = 3;

	types:
		 channel Channel extends ned.DatarateChannel {
			delay = 15ms;
			datarate = 1Gbps;
		}
	submodules:
		Node[36]: Nodes;
	connections allowunconnected:
		for p=0..3, for e=0..1, for s=0..1{
			Node[int(8*p + e*4/2 + s)].out++ --> Channel --> Node[int(8*p + 4*4/4 + e)].in++;
			Node[int(8*p + e*4/2 + s)].in++ <-- Channel <-- Node[int(8*p + 4*4/4 + e)].out++;
		}
		for p=0..3, for e=0..1, for s=2..3{
			Node[int(8*p + 4*4/4 + e)].out++ --> Channel --> Node[int(8*p + 4*4/4 + s)].in++;
			Node[int(8*p + 4*4/4 + e)].in++ <-- Channel <-- Node[int(8*p + 4*4/4 + s)].out++;
		}
		for p=0..3, for e=0..1, for s=0..1{
			Node[int(8*p + 4*4/4 + 4/2 + e)].out++ --> Channel --> Node[int(e*4/4 + s + 32)].in++;
			Node[int(8*p + 4*4/4 + 4/2 + e)].in++ <-- Channel <-- Node[int(e*4/4 + s + 32)].out++;
		}
}