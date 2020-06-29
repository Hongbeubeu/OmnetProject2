#include <omnetpp.h>
#include <queue>
//#include <limits>
#include <fstream>
#include "message_m.h"
#include "FatTreeGraph.h"
#include "FatTreeRoutingAlgorithm.h"

using namespace omnetpp;
using namespace std;
class Nodes: public cSimpleModule{
private:
    int EXB_SIZE;
    int type;
    int k;
    int BUFFER_COUNTER;
    int lastMessageId;
    int destination;
    int sumMsg;
    int *receivedMsgCount;
    int intervalCount = 0;
    double TIMEOUT;
    double TIME_INTERVAL;
    double TIME_GEN_MSG;
    double CHANNEL_DELAY;
    double CREDIT_DELAY;
    double TIME_OPERATION_OF_SWITCH;
    int arrayLength;

    FatTreeRoutingAlgorithm* ftra;
    FatTreeGraph fatTreeGraph;
    map<int, queue<cMessage *>> ENB, EXB;
    map<int, int> sizeOfNextENB; //lưu lại số buffer còn trống tại ENB tại Node kế tiếp
    map<int, bool> channelStatus; //biến sử dụng để check xem đường truyền rảnh hay bận tại từng cổng
    queue<int> SQ, EXB_SD; //Source queue và exit buffer của sender
    queue<int> sendTo;

protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void finish() override;

    //wrap function sử dụng cho từng loại thiết bị sender, switch, receiver
    void senders(cMessage *msg);
    void switches(cMessage *msg);
    void receivers(cMessage *msg);

    //các hàm sử dụng cho sender
    void generateMessage();
    void sendToExitBuffer_SD();
    void sendToSwitch();

    //các hàm sử dụng cho switch
    void sendToExitBuffer_SW();
    bool checkENB();
    void sendToNextNode();
    //void sendSignalToNeighbor(int);
    void sendSignalToIncBuff(int);

    //các hàm sử dụng cho receiver
    void sendSignalToIncBuff_RC();
};

Define_Module(Nodes);

void Nodes::initialize(){
    EXB_SIZE = par("EXB_SIZE").intValue();
    TIMEOUT = par("TIMEOUT").doubleValue();
    TIME_INTERVAL = par("TIME_INTERVAL").doubleValue();
    CREDIT_DELAY = par("CREDIT_DELAY").doubleValue();
    TIME_GEN_MSG = par("TIME_GEN_MSG").doubleValue();
    CHANNEL_DELAY = par("CHANNEL_DELAY").doubleValue();
    TIME_OPERATION_OF_SWITCH = par("TIME_OPERATION_OF_SWITCH").doubleValue();
    type = par("type").intValue();
    if (type == 2){
        k = par("k").intValue();
        fatTreeGraph = FatTreeGraph(k);
        ftra = new FatTreeRoutingAlgorithm(fatTreeGraph, true);
        for (int i = 0; i < k; i++){
            cGate *g = gate("out", i);
            int index = g->getNextGate()->getOwnerModule()->getIndex();
            sendTo.push(index);
            sizeOfNextENB[index] = EXB_SIZE;
            channelStatus[index] = false;
        }
        scheduleAt(0 + TIME_OPERATION_OF_SWITCH, new cMessage("nextPeriod"));
        scheduleAt(0 + CHANNEL_DELAY, new cMessage("send"));
    }
    if (type == 1){
        destination = par("des").intValue();
        BUFFER_COUNTER = EXB_SIZE;
        lastMessageId = -1;
        scheduleAt(0, new cMessage("generate"));
        scheduleAt(0 + CHANNEL_DELAY, new cMessage("send"));
    }
    if (type == 3){
        sumMsg = 0;
        arrayLength = TIMEOUT / TIME_INTERVAL;
        receivedMsgCount = new int[arrayLength];
        memset(receivedMsgCount, 0, arrayLength * sizeof(int));
        scheduleAt(0, new cMessage("nextInterval"));
    }
}

void Nodes::handleMessage(cMessage *msg){
    if(type == 1){
        senders(msg); // xử lý gói tin đến nút gửi
    }else if(type == 2){
        switches(msg); //xử lý gói tin đến switch
    }else if (type == 3){
        receivers(msg); // xử lý gói tin đến nút đích
    }
}

void Nodes::senders(cMessage *msg){
    if(simTime() >= TIMEOUT){
        delete msg;
        return;
    }

    //sinh gói tin định kỳ
    if(!strcmp(msg->getName(), "generate")){
        generateMessage();
        if(EXB_SD.size() < EXB_SIZE)
            sendToExitBuffer_SD();
        scheduleAt(simTime() + TIME_GEN_MSG, msg);
    }

    //gửi gói tin sang switch
    if(!strcmp(msg->getName(), "send")){
        if(BUFFER_COUNTER > 0 && EXB_SD.size() > 0){
            sendToSwitch();
            sendToExitBuffer_SD();
            --BUFFER_COUNTER;
        }
        scheduleAt(simTime() + CHANNEL_DELAY, msg);
    }

    //tăng buffer của nút kế tiếp sau 1 khoảng thời gian CREDIT_DELAY
    if(!strcmp(msg->getName(), "inc buffer")){
        scheduleAt(simTime() + CREDIT_DELAY, new cMessage("incBuff"));
        delete msg;
    }

    //tăng buffer
    if(!strcmp(msg->getName(), "incBuff")){
        ++BUFFER_COUNTER;
        delete msg;
    }
}

//Sinh gói tin
void Nodes::generateMessage(){
    SQ.push(++lastMessageId);
}

//gửi gói tin tới exitbuffer
void Nodes::sendToExitBuffer_SD(){
    if( !SQ.empty()){
        int msgId = SQ.front();
        SQ.pop();
        EXB_SD.push(msgId);
    }
}

//gửi gói tin tới switch
void Nodes::sendToSwitch(){
    int sendMsgId = EXB_SD.front();
    EXB_SD.pop();
    sendMsg *sMsg = new sendMsg("sender to receiver");
    sMsg->setSource(getIndex());
    sMsg->setDestination(destination);
    sMsg->setPayload(sendMsgId);
    send(sMsg, "out", 0);
}

/**
 * nhận gói tin mà các node gửi đến
 * lưu message vào ENB tương ứng
 * gửi thông báo đường truyền rảnh cho node trước
 */
void Nodes::switches(cMessage *msg){
    if(simTime() >= TIMEOUT){
        delete msg;
        EV << "Time out" << endl;
        return;
    }
    const char * eventName = msg->getName();
    //chuyển gói tin vào ENB tương ứng khi switch nhận được gói tin
    if(!strcmp(eventName, "sender to receiver")){
        int index = msg->getSenderModule()->getIndex();
        queue<cMessage *> temp;
        if (ENB[index].empty()){
            temp.push(msg);
            ENB[index] = temp;
        } else {
            temp = ENB[index];
            temp.push(msg);
            ENB[index] = temp;
        }
    }
    //Kiểm tra gói tin muốn sang cổng EXB theo chu kỳ hoạt động của switch
    if(!strcmp(eventName, "nextPeriod")){
        if(ENB.size() > 0){
            sendToExitBuffer_SW();
        }
        scheduleAt(simTime() + TIME_OPERATION_OF_SWITCH, msg);
    }
    //tăng buffer khi nhận được tín hiệu từ nút kề tương ứng sau khoảng thời gian CREDIT_DELAY
    if (!strcmp(eventName, "inc buffer")){
        cMessage *sendMsg = new cMessage("incbuff");
        sendMsg->addPar("preNode");
        sendMsg->par("preNode").setLongValue(msg->getSenderModule()->getIndex());
        scheduleAt(simTime() + CREDIT_DELAY, sendMsg);
        delete msg;
    }
    //tăng buffer
    if (!strcmp(eventName, "incbuff")){
        int index = msg->par("preNode").longValue();
        ++sizeOfNextENB[index];
        delete msg;
    }
    //gửi gói tin tới nút kế tiếp
    if(!strcmp(eventName, "send")){
        sendToNextNode();
        scheduleAt(simTime() + CHANNEL_DELAY, msg);
    }
}
//Tìm gói tin muốn chuyển sang exitbuffer
void Nodes::sendToExitBuffer_SW(){
    sendMsg *ttmsg;
    queue<cMessage *> temp;
    cMessage *mess;
    for(int i = 0; i < k; i++){
        cGate *g = gate("out", i);
        int index = g->getNextGate()->getOwnerModule()->getIndex();
        int id = numeric_limits<int>::max();
        int location = -1;
        for(map<int, queue<cMessage *>>::iterator it = ENB.begin(); it != ENB.end(); it++){
            temp = it->second;
            if(!temp.empty()){
                ttmsg = check_and_cast<sendMsg *>(temp.front());
                int payload = ttmsg->getPayload();
                if(ftra->next(ttmsg->getSource(), getIndex(), ttmsg->getDestination()) == index){
                    if (payload < id){
                        id = payload;
                        location = it->first;
                    }
                }
            }
        }
        if(location > -1){
           mess = ENB[location].front();
           if(EXB[index].size() < EXB_SIZE){
               EXB[index].push(mess);
               sendSignalToIncBuff(location);
               ENB[location].pop();
           }
        }
    }
}
//gửi tín hiệu tăng buffer cho nút tương ướng khi có gói tin ra khỏi ENB ứng với nút đó
void Nodes::sendSignalToIncBuff(int nodeIndex){
    int index;
    for (int i = 0; i < k; i++){
        cGate *g = gate("out", i);
        index = g->getNextGate()->getOwnerModule()->getIndex();
        if(index == nodeIndex){
            send(new cMessage("inc buffer"), "out", i);
            break;
        }
    }
}
//gửi gói tin sang nút tiếp theo
void Nodes::sendToNextNode(){
    for (int i = 0; i < k; i++){
        cGate *g = gate("out", i);
        int index = g->getNextGate()->getOwnerModule()->getIndex();
        if(sizeOfNextENB[index] > 0){
            if (!EXB[index].empty()){
                sizeOfNextENB[index]--;
                cMessage * mess = EXB[index].front()->dup();
                send(mess, "out", i);
                delete EXB[index].front();
                EXB[index].pop();
            }
        }
    }
}
//xử lý tin nhắn đến nút đích
void Nodes::receivers(cMessage *msg){
    if (simTime() >= TIMEOUT){
        delete msg;
        return;
    }
    const char * eventName = msg->getName();
    if(!strcmp(eventName, "sender to receiver")){
        sendSignalToIncBuff_RC();
        sumMsg++;
        receivedMsgCount[intervalCount]++;
        delete msg;
    }
    if (!strcmp(msg->getName(), "nextInterval")) {
        intervalCount++;
        scheduleAt(simTime() + TIME_INTERVAL, msg);
    }
}

void Nodes::sendSignalToIncBuff_RC(){
    send(new cMessage("inc buffer"), "out", 0);
}
//tính toán số gói tin nhận được theo từng interval lưu vào file
void Nodes::finish(){
    if(type == 3){
        fstream fileOutput, fileInput;
        int preArr[arrayLength];
        for (int i = 0; i < arrayLength; i++)
            preArr[i] = 0;
        fileInput.open(("E:/OmnetLab/Lab6/Lab6-5/output.txt"), ios::in);
        if(fileInput){
            int i = 0;
            while (fileInput >> preArr[i])
                i++;
        }
        fileInput.close();
        fileOutput.open(("E:/OmnetLab/Lab6/Lab6-5/output.txt"), ios::out);
        for (int i = 0; i < arrayLength; i++) {
            fileOutput << receivedMsgCount[i] + preArr[i] << " ";
        }
        fileOutput << endl;
        fileOutput.close();
        cout << "Node " << getIndex() << " received : "  << sumMsg << " message." <<  endl;
    }

}
