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
    int k;
    int BUFFER_COUNTER;
    int lastMessageId;
    int destination;
    int sumMsg;
    int counter = 0;
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
    map<int, int> sizeOfNextENB; //lưu lại số buffer còn trống tại ENB tại Node kế tiếp
    map<int, bool> channelStatus; //biến sử dụng để check xem đường truyền rảnh hay bận tại từng cổng
    queue<int> SQ, EXB_SD; //Source queue và exit buffer của sender
    queue<int> sendTo;

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
    void sendToNextNode(int);
    void sendSignalToNeighbor(int);
    void sendSignalToIncBuff(int);

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
        scheduleAt(0 + TIME_INTERVAL, new cMessage("nextPeriod"));
        scheduleAt(0 + TIME_OPERATION_OF_SWITCH, new cMessage("send"));

    }
    if (type == 1){
        CHANNEL_DELAY = par("CHANNEL_DELAY").doubleValue();
        destination = par("des").intValue();
        BUFFER_COUNTER = EXB_SIZE;
        lastMessageId = -1;
        scheduleAt(0, new cMessage("generate"));
        scheduleAt(0, new cMessage("send"));
    }
    if (type == 3){
        sumMsg = 0;
    }

}

void Nodes::handleMessage(cMessage *msg){
    if(type == 1){
        senders(msg);
    }else if(type == 2){
        switches(msg);
    }else if (type == 3){
        receivers(msg);
    }
}

void Nodes::senders(cMessage *msg){
    if(simTime() >= TIMEOUT)
        return;

    //cModule *nextGate = gate("out", 0)->getNextGate()->getOwnerModule();
    //EV << getIndex() <<"-" << nextGate->getFullPath() << endl;
    if(!strcmp(msg->getName(), "generate")){
        generateMessage();
        if(EXB_SD.size() < EXB_SIZE)
            sendToExitBuffer_SD();
        scheduleAt(simTime() + TIME_GEN_MSG, msg);
    }

    if(!strcmp(msg->getName(), "send")){
        //EV << "EXB size: " << EXB_SD.size() << endl;
        if(BUFFER_COUNTER > 0 && EXB_SD.size() > 0){
            sendToSwitch();
            sendToExitBuffer_SD();
            --BUFFER_COUNTER;
        }

        scheduleAt(simTime() + TIME_OPERATION_OF_SWITCH, msg);
    }

    if(!strcmp(msg->getName(), "inc buffer")){
        scheduleAt(simTime() + CREDIT_DELAY, new cMessage("incBuff"));
        delete msg;
    }

    if(!strcmp(msg->getName(), "incBuff")){
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
//
//    for(int i = 0; i < k; i++){
//        cGate *g = gate("out", i);
//        int index = g->getNextGate()->getOwnerModule()->getIndex();
//        EV << g->getFullPath() << " : " << sizeOfNextENB[index] << endl;
//    }
    const char * eventName = msg->getName();

    /**
     * nhận gói tin mà các node gửi đến
     * lưu message vào ENB tương ứng
     * gửi thông báo đường truyền rảnh cho node trước
     */
    if(!strcmp(eventName, "sender to receiver")){
        //counter++;
        //cout << getFullName() << " : counter = " << counter << endl;
        int index = msg->getSenderModule()->getIndex();
        EV << "sender: " << index << endl;
        sendSignalToNeighbor(index);
        queue<cMessage *> temp;
        if (ENB[index].empty()){
            temp.push(msg);
            ENB[index] = temp;
        }else{
            temp = ENB[index];
            temp.push(msg);
            ENB[index] = temp;
        }
    }

    //Kiểm tra gói tin muốn sang cổng EXB theo chu kỳ hoạt động của switch
    if(!strcmp(eventName, "nextPeriod")){
        //EV << "nextPeriod " << ENB.size()  << endl;
        if(ENB.size() > 0){
            sendToExitBuffer_SW();
        }
        scheduleAt(simTime() + TIME_INTERVAL, msg);
    }

    //Set channel status if send success message
    if(!strcmp(eventName, "signal")){
        int index = msg->getSenderModule()->getIndex();
        channelStatus[index] = false;
        delete msg;
    }

    if (!strcmp(eventName, "inc buffer")){
        int index = msg->getSenderModule()->getIndex();
        //EV << "sizeOfNextENB " << index << " : " << sizeOfNextENB[index] << endl;
        ++sizeOfNextENB[index];
        delete msg;
    }

    //Send message to next node
    if(!strcmp(eventName, "send")){
        do{
            int index = sendTo.front();
            if(!channelStatus[index]){
                if(sizeOfNextENB[index] > 0){
                    sendToNextNode(index);
                    sizeOfNextENB[index]--;
                    channelStatus[index] = true;
                    sendTo.pop();
                    sendTo.push(index);
                    break;
                }
            }
            sendTo.pop();
            sendTo.push(index);
        }while(1);
//        for(int i = 0; i < k; i++){
//            cGate *g = gate("out", i);
//            int index = g->getNextGate()->getOwnerModule()->getIndex();
//            //EV << "ENB["<< index << "]" <<".size = " << ENB[index].size() << endl;
//            //EV << "sizeOfNextENB[" << index << "] = " << sizeOfNextENB[index] << endl;
//            if (!channelStatus[index]){
//                if(sizeOfNextENB[index] > 0){
//                    sendToNextNode(index);
//                    sizeOfNextENB[index]--;
//                    channelStatus[index] = true;
//                    EV << "toang o day?" << endl;
//                    //break;
//                }
//            }
//        }
        scheduleAt(simTime() + TIME_OPERATION_OF_SWITCH, msg);
    }

}

void Nodes::sendToExitBuffer_SW(){
    sendMsg *ttmsg;
    queue<cMessage *> temp, temp1;
    cMessage *mess;
    for(int i = 0; i < k; i++){
        cGate *g = gate("out", i);
        int index = g->getNextGate()->getOwnerModule()->getIndex();
        int id = numeric_limits<int>::max();
        int location = -1;
        //EV << "ENB["<< index << "].size = " << ENB[index].size() << endl;
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
           if(EXB[index].empty()){
               temp1.push(mess);
           } else {
               temp1 = EXB[index];
               if(EXB[index].size() < EXB_SIZE)
                   temp1.push(mess);
           }
           EXB[index] = temp1;
           sendSignalToIncBuff(location);
           ENB[location].pop();
        }
    }
}

void Nodes::sendSignalToNeighbor(int nodeIndex){
    int index;
    for (int i = 0; i < k; i++){
        cGate *g = gate("out", i);
        index = g->getNextGate()->getOwnerModule()->getIndex();
        if(index == nodeIndex){
            send(new cMessage("signal"), "out", i);
            break;
        }
    }
}

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

void Nodes::sendToNextNode(int nodeIndex){
    //EV << "send to " << nodeIndex << endl;
    cMessage * mess;
    for (int i = 0; i < k; i++){
        cGate *g = gate("out", i);

        int index = g->getNextGate()->getOwnerModule()->getIndex();
        //EV <<"index = " << index << endl;
        //EV <<"nodeIndex = " << nodeIndex << endl;
        if(index == nodeIndex){
            //EV << EXB[index].empty() << endl;
            if (!EXB[index].empty()){
                mess = EXB[index].front();
                EV << "index :" << index << endl;
                EV << "Node Index : " << getIndex() << endl;
                EV << "sender :" << mess->getSenderModule()->getIndex() << endl;
                EV << "Owner" << mess->getOwner()->getFullPath() << endl;
                cout << "Node[" << getIndex() << "].port out: " << i << " -> " << "node: " << index << endl;
                send(mess, "out", i);
                EXB[index].pop();
                //delete ttmsg;
                break;
            }
            break;
        }
    }
    //delete mess;
}
void Nodes::receivers(cMessage *msg){
    if (simTime() >= TIMEOUT){
        return;
    }
    const char * eventName = msg->getName();
    if(!strcmp(eventName, "sender to receiver")){
        //sendSignalToSwitch();
        EV << "Received msg" << endl;
        sumMsg++;
        //receivedMsgCount[intervalCount]++;
        delete msg;
        cout << getIndex() << ": " << sumMsg << endl;
    }


//    if (strcmp(msg->getName(), "nextInterval") == 0) {
//        intervalCount++;
//        scheduleAt(simTime() + TIME_INTERVAL, msg);
//    }
}
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
