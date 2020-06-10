#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <vector>
#include <algorithm>
#include <ctime>
using namespace std;
int main(int argc, char* argv[]){
    if(argc != 2){
        printf("Not found agruments\n");
        return 0;
    }
    FILE *fptr;
    fptr = fopen("E:/Omnetpp/Test/fattree.ned","w");
    int k = atoi(argv[1]);
    int numServers = k*k*k/4;
    int numSwitchs = 5*k*k/4;
    int numPodSwitches = k*k;
    int numCores = k*k/4;
    int numNodes = numServers + numSwitchs;
    int numEachPod = k * k / 4 + k;
    fprintf(fptr,"simple Node\n{\n\tparameters:\n\t\t@display(\"i=device/pc\");\n\t\tint type;\n\t\tint des = default(-1);\n\t\tint EXB_SIZE;\n\t\tdouble TIMEOUT;\n\t\tdouble TIME_INTERVAL;\n\t\tdouble CHANNEL_DELAY;\n\t\tdouble CREDIT_DELAY;\n\t\tdouble TIME_GEN_MSG;\n\t\tdouble TIME_OPERATION_OF_SWITCH;\n\tgates:\n\t\tinput in[%d];\n\t\toutput out[%d];\n}\n\n",k,k);
    fprintf(fptr,"network fattree\n{\n\ttypes:\n\t\t channel Channel extends ned.DelayChannel {\n\t\t\tdelay = 15ms;\n\t\t\tdatarate = 1Gbps;\n\t\t}\n");
    fprintf(fptr, "\tparameters:\n");

    for (int p = 0; p < k; p++) {
        int offset = numEachPod * p;
        //set type to edge switches
        for (int e = 0; e < k / 2; e++) {
            int edgeSwitch = offset + k * k / 4 + e;
            fprintf(fptr, "\t\tNode[%d].type = 2;\n",edgeSwitch);
        }
        //set type to agg switches
        for (int a = k / 2; a < k; a++) {
            int aggSwitch = offset + k * k / 4 + a;
            fprintf(fptr, "\t\tNode[%d].type = 2;\n",aggSwitch);
        }   
    }
    //set type to core switches
    for (int a = 0; a < k / 2; a++) {
        for (int c = 0; c < k / 2; c++) {
            int coreSwitch = a * k / 2 + c + numPodSwitches + numServers;
            fprintf(fptr, "\t\tNode[%d].type = 2;\n", coreSwitch);
        }
    }
    
    //list host to hosts
    vector<int> hosts;
    for (int p = 0; p < k; p++) {
        int offset = numEachPod * p;
        for (int e = 0; e < k / 2; e++) {
            for (int s = 0; s < k / 2; s++) {
                int server = offset + e * k / 2 + s;
                hosts.push_back(server);
            }
        }
    }

    srand ( unsigned ( time(0) ) );
    //generate random list of host
    random_shuffle (hosts.begin(), hosts.end());

    //generate random pair host
    vector< pair<int,int> > genRandomPairs;
    int counter = 0;
    int prehost;
    for (auto nexthost : hosts) {
        if (++counter == 2) {
            counter = 0;
            genRandomPairs.push_back(pair<int,int>(prehost, nexthost));
            fprintf(fptr, "\t\tNode[%d].type = 1;\n", prehost);
            fprintf(fptr, "\t\tNode[%d].type = 3;\n", nexthost);
        }
        prehost = nexthost;
    }

    fprintf(fptr,"\tsubmodules:\n\t\tNodes[%d]: Node;\n", numNodes);
    fprintf(fptr,"\tconnections:\n");
    fprintf(fptr,"\t\tfor p=0..%d, for e=0..%d, for s=0..%d{\n", k-1, k/2-1, k/2-1);
    fprintf(fptr,"\t\t\tNodes[int(%d*p + e*%d/2 + s)].out++ --> Channel --> Nodes[int(%d*p + %d*%d/4 + e)].in++;\n", numEachPod, k, numEachPod, k, k);
    fprintf(fptr,"\t\t\tNodes[%d*p + e*%d/2 + s].in++ <-- Channel <-- Nodes[%d*p + %d*%d/4 + e].out++;\n", numEachPod, k, numEachPod, k, k);
    fprintf(fptr,"\t\t}\n");
    fprintf(fptr,"\t\tfor p=0..%d, for e=0..%d, for s=%d..%d{\n", k-1, k/2-1, k/2, k-1);
    fprintf(fptr,"\t\t\tNodes[int(%d*p + %d*%d/4 + e)].out++ --> Channel --> Nodes[int(%d*p + %d*%d/4 + s)].in++;\n", numEachPod, k, k, numEachPod, k, k);
    fprintf(fptr,"\t\t\tNodes[int(%d*p + %d*%d/4 + e)].in++ <-- Channel <-- Nodes[int(%d*p + %d*%d/4 + s)].out++;\n", numEachPod, k, k, numEachPod, k, k);
    fprintf(fptr,"\t\t}\n");
    fprintf(fptr,"\t\tfor p=0..%d, for e=0..%d, for s=0..%d{\n", k-1, k/2-1, k/2-1);
    fprintf(fptr,"\t\t\tNodes[int(%d*p + %d*%d/4 + %d/2 + e)].out++ --> Channel --> Nodes[int(e*%d/4 + s + %d)].in++;\n", numEachPod, k, k, k, k, numPodSwitches + numServers);
    fprintf(fptr,"\t\t\tNodes[int(%d*p + %d*%d/4 + %d/2 + e)].in++ <-- Channel <-- Nodes[int(e*%d/4 + s + %d)].out++;\n", numEachPod, k, k, k, k, numPodSwitches + numServers);
    fprintf(fptr,"\t\t}\n}");
    fclose(fptr);
    return 0;
}