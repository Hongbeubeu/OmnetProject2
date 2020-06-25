#include <omnetpp.h>
#include <queue>
#include <limits>
#include "message_m.h"
#include "FatTreeGraph.h"
#include "FatTreeRoutingAlgorithm.h"

using namespace omnetpp;
using namespace std;
class Nodes: public cSimpleModule{
private:
    int EXB_SIZE;
    int type;
    int BUFFER_COUNTER;
    int lastMessageId;
    int destination;
    double TIMEOUT;
    double TIME_INTERVAL;
    double TIME_GEN_MSG;
    double CHANNEL_DELAY;
    double CREDIT_DELAY;
    double TIME_OPERATION_OF_SWITCH;
    bool isChannelBussy;

    FatTreeRoutingAlgorithm* ftra;
    FatTreeGraph fatTreeGraph;
    map<int, queue<cMessage *>> ENB, EXB;
//    queue<cMessage *> ENB[4], EXB_SW[4]; // enchance buffer và exit buffer của switch
    queue<int> SQ, EXB_SD; //Source queue và exit buffer của sender


protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;

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
    void sendSignalToNeighbor(int);

    //các hàm sử dụng cho receiver
    void sendSignalToSwitch();
};

Define_Module(Nodes);

void Nodes::initialize(){
    EXB_SIZE = par("EXB_SIZE").intValue();
    TIMEOUT = par("TIMEOUT").doubleValue();
    TIME_INTERVAL = par("TIME_INTERVAL").doubleValue();
    CREDIT_DELAY = par("CREDIT_DELAY").doubleValue();
    TIME_GEN_MSG = par("TIME_GEN_MSG").doubleValue();
    TIME_OPERATION_OF_SWITCH = par("TIME_OPERATION_OF_SWITCH").doubleValue();
    type = par("type").intValue();
    isChannelBussy = false;
    if (type == 2){
        int k = 4;
        fatTreeGraph = FatTreeGraph(k);
        ftra = new FatTreeRoutingAlgorithm(fatTreeGraph, true);
        scheduleAt(0 + TIME_OPERATION_OF_SWITCH, new cMessage("send"));
        scheduleAt(0 + TIME_INTERVAL, new cMessage("nextPeriod"));
    }
    if (type == 1){
        CHANNEL_DELAY = par("CHANNEL_DELAY").doubleValue();
        destination = par("des").intValue();
        BUFFER_COUNTER = EXB_SIZE;
        lastMessageId = -1;
        scheduleAt(0, new cMessage("generate"));
        scheduleAt(0, new cMessage("send"));
    }
    if (type > 1)
        scheduleAt(0 + TIME_INTERVAL, new cMessage("nextInterval"));
}

void Nodes::handleMessage(cMessage *msg){
    if(type == 1){
        senders(msg);
    }else if(type == 2){
        switches(msg);
    }else{
        //receivers(msg);
    }
}

void Nodes::senders(cMessage *msg){
    if(simTime() >= TIMEOUT)
        return;
    //sendMsg *ttmsg = check_and_cast<sendMsg *>(msg);
    cModule *nextGate = gate("out", 0)->getNextGate()->getOwnerModule();
    EV << getIndex() <<"-" << nextGate->getFullPath() << endl;
    if(strcmp(msg->getName(), "generate") == 0){
        generateMessage();
        if(EXB_SD.size() < EXB_SIZE)
            sendToExitBuffer_SD();
        scheduleAt(simTime() + TIME_GEN_MSG, msg);
    }

    //if(strcmp(msg->getName(), "send") == 0){
        if(BUFFER_COUNTER > 0 && EXB_SD.size() > 0){
            sendToSwitch();
            sendToExitBuffer_SD();
            --BUFFER_COUNTER;
        }
        //scheduleAt(simTime() + CHANNEL_DELAY, msg);
    //}

    if(strcmp(msg->getName(), "signal") == 0){
        scheduleAt(simTime() + CREDIT_DELAY, new cMessage("incBuff"));
        delete msg;
    }

    if(strcmp(msg->getName(), "incBuff") == 0){
        ++BUFFER_COUNTER;
        delete msg;
    }
}

void Nodes::generateMessage(){
    SQ.push(++lastMessageId);
    EV << "generated message id = " << lastMessageId << endl;
}

void Nodes::sendToExitBuffer_SD(){
    if( !SQ.empty()){
        int msgId = SQ.front();
        SQ.pop();
        EXB_SD.push(msgId);
    }
}

void Nodes::sendToSwitch(){
    int sendMsgId = EXB_SD.front();
    EXB_SD.pop();
    sendMsg *sMsg = new sendMsg("sender to receiver");
    sMsg->setSource(getIndex());
    sMsg->setDestination(destination);
    sMsg->setPayload(sendMsgId);
    send(sMsg, "out", 0);
}


void Nodes::switches(cMessage *msg){
    if(simTime() >= TIMEOUT){
        EV << "Time out" << endl;
        return;
    }

    const char * eventName = msg->getName();

    /**
     * lấy id của gói tin mà các sender gửi lên
     * lưu các id vào ENB tương ứng
     * sinh sự kiện gửi gói tin từ ENB sang EXB sau 1 chu kỳ hoạt động của switch = chu kỳ sinh gói tin
     */
    if(strcmp(eventName, "sender to receiver") == 0){
        int index = msg->getSenderModule()->getIndex();
        //ENB[index] = msg;
        if (ENB[index].empty()){
            queue<cMessage *> temp;
            temp.push(msg);
            ENB[index] = temp;
        }else{
            queue<cMessage *> temp;
            temp = ENB[index];
            temp.push(msg);
            ENB[index] = temp;
        }
//        EV << index << " " << ENB[index].front()->getFullPath() << endl;
//        sendMsg *ttmsg = check_and_cast<sendMsg *>(msg);
//        int src = ttmsg->getSource();
//        int des = ttmsg->getDestination();
//        EV << src << "->" << des << " ";
//
//        EV << ftra->next(src, getIndex(), des) << endl;
//        for (int node : ftra->path(src, des).path)
//            EV << node << " ";
//        EV << ttmsg->getSource() << endl;
//        EV << ttmsg->getDistination() << endl;
//        EV << ttmsg->getPayload() << endl;
        //int index = msg->getSenderModule()->getIndex();
        //EV << eventName << endl;
//        int payload = msg->par("msgId").longValue();
//        queue<cMessage *> temp;
//        temp.push(msg);
//        ENB[index] = temp;
//        EV << "switches " << getFullPath() << ":" << ENB[index].front()->getName() << endl;
        return;
    }

    //Kiểm tra gói tin muốn sang cổng EXB theo chu kỳ hoạt động của switch
    if(strcmp(eventName, "nextPeriod")){
        if(EXB.size() < EXB_SIZE){
            sendToExitBuffer_SW();
        }
    }

    //Set channel status if send success message
//    if(strcmp(eventName, "signal") == 0){
//        isChannelBussy = false;
//        delete msg;
//    }

    //Send message to receiver

//    if(strcmp(eventName, "send") == 0){
//        if(!EXB.empty()){
//            if(!isChannelBussy){
//                //sendToNextNode();
//                isChannelBussy = true;
//            }
//        }
//        scheduleAt(simTime() + TIME_INTERVAL, msg);
//    }
}

void Nodes::sendToExitBuffer_SW(){

    for(int i = 0; i < 4 ; i++){
        cGate *g = gate("out", i);
        int index = g->getNextGate()->getOwnerModule()->getIndex();
        int id = numeric_limits<int>::max();
        int location = -1;
        for(map<int, queue<cMessage *>>::iterator it = ENB.begin(); it != ENB.end(); it++){
            queue<cMessage *> temp = it->second;
            sendMsg *ttmsg = check_and_cast<sendMsg *>(temp.front());
            int payload = ttmsg->getPayload();
            if(ftra->next(ttmsg->getSource(), getIndex(), ttmsg->getDestination()) == index){
                if ( payload < id){
                    id = payload;
                    location = it->first;
                }
            }
            //EV << ttmsg->getSource() << " -> " << ttmsg -> getDestination() << " : " << ttmsg->getPayload() << endl;
        }
        //cMessage * t = ENB[location].front();
        if(location > -1){
            cMessage *mess = ENB[location].front();
            EXB[index].push(mess);
            sendMsg *ttmsg = check_and_cast<sendMsg *>(ENB[location].front());
            ENB[location].pop();
            sendSignalToNeighbor(location);
        }
    }
}

//void Nodes::Receivers(cMessage *msg){
//    if (simTime() >= TIMEOUT){
//        return;
//    }
//
//    if (strcmp(msg->getName(), "sender to receiver msg") == 0) {
//        sendSignalToSwitch();
//        EV << "Received msg" << endl;
//        sumMsg++;
//        receivedMsgCount[intervalCount]++;
//        delete msg;
//
//    }
//
//    if (strcmp(msg->getName(), "nextInterval") == 0) {
//        intervalCount++;
//        scheduleAt(simTime() + TIME_INTERVAL, msg);
//    }
//}
///**
// * gửi thông báo ENB tương ứng có chỗ trống
// * @input số hiệu cổng gửi signal
// * @return không
// */
//
//bool Nodes::checkENB(){
//    for (int i = 0; i < 3; i++){
//        if(!ENB[i].empty())
//            return true;
//    }
//    return false;
//}
//
//void Nodes::sendSignalToSender(int port){
//    send(new cMessage("signal"), "out", port);
//}
//
///**
// * gửi gói tin từ ENB sang EXB
// * @input không
// * @return không
// */
//
//void Nodes::sendToExitBuffer(){
//    int id = numeric_limits<int>::max();
//    int port = 0;
//    for ( int i = 0; i < 3; i++){
//        if (!ENB[i].empty()){
//            if(ENB[i].front() < id){
//                id = ENB[i].front();
//                ENB[i].pop();
//                port = i;
//            }
//        }
//    }
//    EXB.push(id);
//    sendSignalToSender(port);
//}
//
///**
// * gửi gói tin đến receiver
// * @return không
// */
//
//void Nodes::sendToReceiver(){
//    int sendMsgId = EXB.front();
//    EXB.pop();
//
//    cMessage *sendMsg = new cMessage("sender to receiver msg");
//
//    cMsgPar *msgParam = new cMsgPar("msgId");
//    msgParam->setLongValue(sendMsgId);
//    sendMsg->addPar(msgParam);
//
//    send(sendMsg, "out", 3);
//}
