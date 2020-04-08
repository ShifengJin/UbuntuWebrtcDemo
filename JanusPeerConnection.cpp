#include "JanusPeerConnection.h"
#include "CommonDefine.h"
#include "CommonFunction.h"

JanusPeerConnection::JanusPeerConnection(JanusWebSocket &socket, QString opaqueId, QObject *parent) : mSocket(socket)
{
    mOpaqueId = opaqueId;

}

JanusPeerConnection::~JanusPeerConnection()
{

}

void JanusPeerConnection::AttachVideoRoom()
{
    Json::Value msg;
    msg["janus"] = "attach";

    msg["plugin"] = "janus.plugin.videoroom";

    msg["opaque_id"] = mOpaqueId.toStdString();
    mSocket.EmitMessage(msg, std::bind(&JanusPeerConnection::onAttachVideoRoom, this, std::placeholders::_1));
}

void JanusPeerConnection::AttachTextRoom()
{
    Json::Value msg;
    msg["janus"] = "attach";

    msg["plugin"] = "janus.plugin.textroom";

    msg["opaque_id"] = mOpaqueId.toStdString();
    mSocket.EmitMessage(msg, std::bind(&JanusPeerConnection::onAttachTextRoom, this, std::placeholders::_1));
}

void JanusPeerConnection::SendSDP(std::string sdp, std::string type)
{
    Json::Value msg;
    msg["janus"] = "message";
    Json::Value body;
    if(mSubscribeId == 0){
        body["request"] = "configure";
        body["audio"] = true;
        body["video"] = true;
    }else{
        body["request"] = "start";
        body["room"] = 1234;
    }

    Json::Value jsep;
    jsep["type"] = type;
    jsep["sdp"] = sdp;

    msg["body"] = body;
    msg["jsep"] = jsep;

    msg["handle_id"] = (double)mHandleId;
    mSocket.EmitMessage(msg, std::bind(&JanusPeerConnection::onSendSDP, this, std::placeholders::_1));

}

void JanusPeerConnection::SendSDPText(std::string sdp, std::string type)
{
    Json::Value msg;
    msg["janus"] = "message";
    Json::Value body;
    body["request"] = "ack";


    Json::Value jsep;
    jsep["type"] = type;
    jsep["sdp"] = sdp;

    msg["body"] = body;
    msg["jsep"] = jsep;

    msg["handle_id"] = (double)mHandleId;
    mSocket.EmitMessage(msg, std::bind(&JanusPeerConnection::onSendSDP, this, std::placeholders::_1));
}

void JanusPeerConnection::onAttachVideoRoom(const Json::Value &recvmsg)
{
    mHandleId = JsonValueToLongLong(recvmsg, "id");

    join();

}

void JanusPeerConnection::onAttachTextRoom(const Json::Value &recvmsg)
{
    mHandleId = JsonValueToLongLong(recvmsg, "id");
    requestSetup();
}



void JanusPeerConnection::join()
{
    Json::Value msg;
    msg["janus"] = "message";
    msg["handle_id"] = (double)mHandleId;

    Json::Value body;
    body["request"] = "join";
    body["room"] = 1234;

    if(mSubscribeId == 0){
        body["ptype"] = "publisher";
        body["display"] = "cc"; // "cc"
    }else{
        body["ptype"] = "subscriber";
        body["feed"] = (double)mSubscribeId;
        body["private_id"] = (double)mPrivateId;
    }
    msg["body"] = body;

    mSocket.EmitMessage(msg, std::bind(&JanusPeerConnection::onJoin, this, std::placeholders::_1));
}

void JanusPeerConnection::requestSetup()
{
    Json::Value msg;
    msg["janus"] = "message";
    msg["handle_id"] = (double)mHandleId;

    Json::Value body;
    body["request"] = "setup";
    msg["body"] = body;

    mSocket.EmitMessage(msg, std::bind(&JanusPeerConnection::onRequestSetup, this, std::placeholders::_1));
}

void JanusPeerConnection::onRequestSetup(const Json::Value &recvmsg)
{

}

void JanusPeerConnection::onJoin(const Json::Value &recvmsg)
{

}

void JanusPeerConnection::onSendSDP(const Json::Value &recvData)
{

}

void JanusPeerConnection::SendCandidate(QString sdpMid, int sdpMLineIndex, QString candidate)
{
    Json::Value msg;
    msg["janus"] = "trickle";

    Json::Value candidateObj;
    if(sdpMid == "end")
    {
        candidateObj["completed"] = true;
    }
    else
    {
        candidateObj["candidate"] = candidate.toStdString();
        candidateObj["sdpMid"] = sdpMid.toStdString();
        candidateObj["sdpMLineIndex"] = sdpMLineIndex;
    }
    msg["candidate"] = candidateObj;

    msg["handle_id"] = (double)mHandleId;
    mSocket.EmitMessage(msg, std::bind(&JanusPeerConnection::onSendCandidate,this,std::placeholders::_1));
}

void JanusPeerConnection::onSendCandidate(const Json::Value &recvmsg)
{

}
