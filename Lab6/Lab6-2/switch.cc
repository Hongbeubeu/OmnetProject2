/*
 * switch.cc
 *
 *  Created on: May 16, 2020
 *      Author: hongt
 */
#include <omnetpp.h>
#include <queue>
#include <limits>

using namespace omnetpp;
using namespace std;
class Switch: public cSimpleModule{
private:
    int EXB_SIZE;
    double TIMEOUT;
    double TIME_INTERVAL;
    double CHANNEL_DELAY;
    double TIME_OPERATION_OF_SWITCH;
    bool isChannelBussy;

    queue<int> ENB[3], EXB;

    void sendToExitBuffer();
    bool checkENB();
    void sendToReceiver();
    void sendSignalToSender(int);
protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
};

Define_Module(Switch);

void Switch::initialize(){
    EXB_SIZE = par("EXB_SIZE").intValue();
    TIMEOUT = par("TIMEOUT").doubleValue();
    TIME_INTERVAL = par("TIME_INTERVAL").doubleValue();
    CHANNEL_DELAY = par("CHANNEL_DELAY").doubleValue();
    isChannelBussy = false;
    TIME_OPERATION_OF_SWITCH = par("TIME_OPERATION_OF_SWITCH").doubleValue();
    //sự kiện gửi gói tin của switch được
    scheduleAt(0 + TIME_OPERATION_OF_SWITCH, new cMessage("send"));
    scheduleAt(0 + TIME_INTERVAL, new cMessage("nextPeriod"));

}

void Switch::handleMessage(cMessage *msg){
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
        int payload = msg->par("msgId").longValue();
        ENB[index].push(payload);
        return;
    }

    //Kiểm tra gói tin muốn sang cổng EXB theo chu kỳ hoạt động của switch
    if(strcmp(eventName, "nextPeriod")){
        if(EXB.size() < EXB_SIZE){
            sendToExitBuffer();
        }
    }

    //Set channel status if send success message
    if(strcmp(eventName, "signal") == 0){
        isChannelBussy = false;
        delete msg;
    }

    //Send message to receiver

    if(strcmp(eventName, "send") == 0){
        if(!EXB.empty()){
            if(!isChannelBussy){
                sendToReceiver();
                isChannelBussy = true;
            }
        }
        scheduleAt(simTime() + TIME_INTERVAL, msg);
    }
}

/**
 * gửi thông báo ENB tương ứng có chỗ trống
 * @input số hiệu cổng gửi signal
 * @return không
 */

bool Switch::checkENB(){
    for (int i = 0; i < 3; i++){
        if(!ENB[i].empty())
            return true;
    }
    return false;
}

void Switch::sendSignalToSender(int port){
    send(new cMessage("signal"), "out", port);
}

/**
 * gửi gói tin từ ENB sang EXB
 * @input không
 * @return không
 */

void Switch::sendToExitBuffer(){
    int id = numeric_limits<int>::max();
    int port = 0;
    for ( int i = 0; i < 3; i++){
        if (!ENB[i].empty()){
            if(ENB[i].front() < id){
                id = ENB[i].front();
                ENB[i].pop();
                port = i;
            }
        }
    }
    EXB.push(id);
    sendSignalToSender(port);
}

/**
 * gửi gói tin đến receiver
 * @return không
 */

void Switch::sendToReceiver(){
    int sendMsgId = EXB.front();
    EXB.pop();

    cMessage *sendMsg = new cMessage("sender to receiver msg");

    cMsgPar *msgParam = new cMsgPar("msgId");
    msgParam->setLongValue(sendMsgId);
    sendMsg->addPar(msgParam);

    send(sendMsg, "out", 3);
}
