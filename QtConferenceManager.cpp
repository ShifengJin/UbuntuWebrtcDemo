#include "QtConferenceManager.h"
#include "AlvaJsonTools.h"
#include <sys/types.h>
#include <unistd.h>
#include <thread>

QtConferenceManager* QtConferenceManager::instance = NULL;

QtConferenceManager::QtConferenceManager(QObject *parent) : QObject(parent)
{
    mJanusVideoRoomManager.RegisterConnectToPeer_CallBack(std::bind(&QtConferenceManager::ConnectToPeer, this,
                                                                    std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));

    mJanusVideoRoomManager.RegisterRemoteSdp_CallBack(std::bind(&QtConferenceManager::onRetmoeSDP, this,
                                                                std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));

    mJanusVideoRoomManager.RegisterRemoteStreamRemove_callBack(std::bind(&QtConferenceManager::onRemoteStreamRemove, this,
                                                                         std::placeholders::_1));
}

QtConferenceManager::~QtConferenceManager()
{

}

QtConferenceManager *QtConferenceManager::GetInstance()
{
    if(instance == NULL){
        instance = new QtConferenceManager();
    }
    return instance;
}

void QtConferenceManager::Login(std::string &server)
{
    mJanusVideoRoomManager.ConnectServer(server);
}

void QtConferenceManager::SetVideoWindows(unsigned long local, QVector<unsigned long> remote)
{
    mLocalWindow = local;
    mRemoteWinds.clear();
    for(auto wind : remote){
        mRemoteWinds.push_back(QPair<WINDOWID, long long>(wind, 0));
    }
}

rtc::scoped_refptr<QtWebrtcRemoteStream> QtConferenceManager::GetLocalWebrtcRemoteStream()
{
    return LocalStream;
}

void QtConferenceManager::addStreamInfo(rtc::scoped_refptr<QtWebrtcRemoteStream> remoteStream)
{
    RemoteStreamInfo stream;
    stream.stream = remoteStream;

    stream.canSendSDP = true;
    stream.canSendCandidate = true;

    long long peerId = remoteStream->id();

    mRemoteStreamInfos[peerId] = stream;
}

void QtConferenceManager::sendICEs(long long id, QVector<QtConferenceManager::iceCandidate> &iceCandidateList)
{
    while(iceCandidateList.count() > 0)
    {
        auto candidate = iceCandidateList.front();
        iceCandidateList.pop_front();
        mJanusVideoRoomManager.SendCandidate(id,candidate.sdp_mid,candidate.sdp_mline_index,candidate.sdp);
    }
}

void QtConferenceManager::ConnectToPeer(long long peerId, bool show, bool isConnect, bool isLocal)
{
    qDebug() << __FILE__ << "  " << __FUNCTION__ << "  " << __LINE__ << "ConnectToPeer";
    qDebug() << __FILE__ << "  " << __FUNCTION__ << "  " << __LINE__ << "isConnect : " << isConnect;
    rtc::scoped_refptr<QtWebrtcRemoteStream> remoteStream = new rtc::RefCountedObject<QtWebrtcRemoteStream>(peerId);
    remoteStream->SetIsLocal(isLocal);

    if(isLocal){
        LocalStream = remoteStream;
    }


    connect(remoteStream, SIGNAL(LocalSDP(long long, QString, QString)), this, SLOT(onLocalSDP(long long, QString, QString)));
    //remoteStream->RegisterSendLocalSDP_CallBack(std::bind(&QtConferenceManager::onLocalSDP, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    //remoteStream->OnSendSDP = std::bind(&QtConferenceManager::onLocalSDP, this, std::placeholders::_1);
    connect(remoteStream, SIGNAL(LocalIceCandidate(long long, QString, int, QString)), this, SLOT(onLocalIceCandidate(long long, QString, int, QString)));
    connect(remoteStream, SIGNAL(IceGatheringComplete(long long)), this, SLOT(onIceGatheringComplete(long long)));

    remoteStream->RegisterSendLocalInfoWhenOpenDataChannel_callBack(std::bind(&QtConferenceManager::onSendLocalInfoWhenOpenDataChannel, this, std::placeholders::_1));

    addStreamInfo(remoteStream);

    if(show){
        for(auto &wind : mRemoteWinds){
            if(wind.second == 0){
                remoteStream->SetWindowID(wind.first);
                wind.second = peerId;
                break;
            }
        }
    }

    if(isConnect){
        remoteStream->ConnectToPeer();
    }
}

void QtConferenceManager::onLocalSDP(long long id, QString sdp, QString type)
{
    std::thread::id threadid = std::this_thread::get_id();
    std::stringstream sin;
    sin << threadid;

    //qDebug() << "================================> onLocalSDP   thread id : " << QString::fromStdString(sin.str());
    //qDebug() << "================================> sdp : " << QString::fromStdString(sdp);
    auto itr = mRemoteStreamInfos.find(id);

    auto  &remoteStreamState  = *itr;
    remoteStreamState.sdp = sdp.toStdString();
    remoteStreamState.sdpType = type.toStdString();
    if(remoteStreamState.canSendSDP){
        mJanusVideoRoomManager.SendSDP(id, sdp.toStdString(), type.toStdString());
    }
}

void QtConferenceManager::onRetmoeIce(long long id, QString sdp_mid, int sdp_mlineindex, QString candidate)
{
    qDebug()<<"===========================================";
    qDebug()<<"======QtConferenceManager::onRetmoeIce=====";
    qDebug()<<"===========================================";

    auto itr = mRemoteStreamInfos.find(id);
    if(itr == mRemoteStreamInfos.end())
    {
        ConnectToPeer(id, true, false, false);
        itr = mRemoteStreamInfos.find(id);
        if(itr == mRemoteStreamInfos.end())
        {
            return;
        }
    }

    auto &remoteStreamState = *itr;
    auto &remoteStream = remoteStreamState.stream;
    remoteStream->AddPeerIceCandidate(sdp_mid, sdp_mlineindex, candidate);
}

void QtConferenceManager::onLocalIceCandidate(long long id, QString sdp_mid, int sdp_mlineindex, QString candidate)
{
    auto itr = mRemoteStreamInfos.find(id);
    if(itr == mRemoteStreamInfos.end()){
        return;
    }

    auto &remoteStreamState = *itr;
    auto &remoteStream = remoteStreamState.stream;

    QVector<iceCandidate> &iceCandidateList = remoteStreamState.iceCandidateList;
    iceCandidateList.push_back(iceCandidate{sdp_mid,sdp_mlineindex,candidate});

    if(remoteStreamState.canSendCandidate)
    {
        sendICEs(remoteStream->id(), iceCandidateList);
    }
}

void QtConferenceManager::onRetmoeSDP(long long id, std::string type, std::string sdp, bool isTextRoom)
{

    auto itr = mRemoteStreamInfos.find(id);
    if(itr == mRemoteStreamInfos.end())
    {
        if(isTextRoom){
            ConnectToPeer(id, false, false, true);
        }else{
            ConnectToPeer(id, true, false, false);
        }

        itr = mRemoteStreamInfos.find(id);
        if(itr == mRemoteStreamInfos.end())
        {
            return ;
        }
    }
    auto &remoteStreamState = *itr;
    auto &remoteStream = remoteStreamState.stream;

    remoteStream->SetPeerSDP(type, sdp);
}

void QtConferenceManager::onRemoteStreamRemove(long long streamId)
{
    auto itr = mRemoteStreamInfos.find(streamId);
    if(itr != mRemoteStreamInfos.end())
    {
        mJanusVideoRoomManager.DisconnectPeer(itr->stream->id());
        itr->stream->DeletePeerConnection();
        mRemoteStreamInfos.erase(itr);
    }
    for(auto &wind : mRemoteWinds)
    {
        if(wind.second == streamId)
        {
            wind.second = 0;
            break;
        }
    }
}

void QtConferenceManager::onIceGatheringComplete(long long id)
{
    onLocalIceCandidate(id, "end", -1, "end");
}

void QtConferenceManager::onSendLocalInfoWhenOpenDataChannel(std::string &data)
{
    Json::Value msg;
    msg["textroom"] = "join";
    std::string transaction = GetRandomString(12).toStdString();
    msg["transaction"] = transaction;
    msg["room"] = 1234;
    std::string username = GetRandomString(12).toStdString();
    msg["username"] = username;
    msg["display"] = "cc";
    data = JsonValueToString(msg);
}
