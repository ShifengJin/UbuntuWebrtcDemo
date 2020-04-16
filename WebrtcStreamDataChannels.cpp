#include "WebrtcStreamDataChannels.h"

WebrtcStreamDataChannels::WebrtcStreamDataChannels(long long mHandleID) : WebrtcStreamBase(mHandleID)
{

}

WebrtcStreamDataChannels::~WebrtcStreamDataChannels()
{
    CloseDataChannel();
    mPeerConnection = nullptr;
    mHandleID = -1;

}

bool WebrtcStreamDataChannels::CreateDataChannel()
{
    struct webrtc::DataChannelInit init;
    init.ordered = true;
    mDataChannel = mPeerConnection->CreateDataChannel("UbuntuWebrtcDemo", &init);
    if(mDataChannel.get()){
        mDataChannel->RegisterObserver(this);
        RTC_LOG(INFO) << "CreateDataChannel successed.";
        return true;
    }
    return false;
}

void WebrtcStreamDataChannels::CloseDataChannel()
{
    if(mDataChannel.get()){
        mDataChannel->UnregisterObserver();
        mDataChannel->Close();
    }
    mDataChannel = nullptr;
}

bool WebrtcStreamDataChannels::SendDataViaDataChannel(const std::string &data)
{
    if(!mDataChannel.get()){
        return false;
    }
    webrtc::DataBuffer buffer(data);
    mDataChannel->Send(buffer);
    return true;
}

void WebrtcStreamDataChannels::RegisterRecvMessageCallBack(RECVMESSAGE_CALLBACK callback)
{
    OnRecvMessage = callback;
}

void WebrtcStreamDataChannels::RegisterSendLocalInfoWhenOpenDataChannelCallBack(SENDLOCALINFOWHENOPENDATACHANNEL_CALLBACK callback)
{
    OnSendLocalInfoWhenOpenDataChannel = callback;
}

bool WebrtcStreamDataChannels::SetPeerSDP(std::string type, std::string sdp)
{
    if(!mPeerConnection.get())
    {
        if(!initializePeerConnection())
        {
            RTC_LOG(LS_ERROR)<< "Failed to initialize our PeerConnection instance";
            return false;
        }
    }
    webrtc::SdpParseError error;
    webrtc::SessionDescriptionInterface* session_description(webrtc::CreateSessionDescription(type, sdp, &error));

    if(!session_description)
    {
        RTC_LOG(WARNING) << "Can't parse received session description message."
                         <<"SdpParseError was:"<<error.description;
        return false;
    }

    mPeerConnection->SetRemoteDescription(
                DummySetSessionDescriptionObserver::Create(), session_description);

    if(session_description->type() ==
            webrtc::SessionDescriptionInterface::kOffer)
    {
        mPeerConnection->CreateAnswer(this, webrtc::PeerConnectionInterface::RTCOfferAnswerOptions(false, false, true, false, true));
    }

    return true;
}

bool WebrtcStreamDataChannels::initializePeerConnection()
{
    if(!createPeerConnection(true))
    {
        RTC_LOG(INFO) << "CreatePeerConnection failed";
        DeletePeerConnection();
    }

    if(CreateDataChannel()){
        qDebug() << "CreateDataChannel successed.";
    }

    return mPeerConnection != nullptr;
}

void WebrtcStreamDataChannels::OnDataChannel(rtc::scoped_refptr<webrtc::DataChannelInterface> data_channel)
{
    data_channel->RegisterObserver(this);
}

void WebrtcStreamDataChannels::OnStateChange()
{
    qDebug() << " ||| ========== OnStateChange ========== |||";
    if (mDataChannel) {
        webrtc::DataChannelInterface::DataState state = mDataChannel->state();
        if (state == webrtc::DataChannelInterface::kOpen) {
            qDebug() << "Data channel is open";

            if(OnSendLocalInfoWhenOpenDataChannel){
                std::string sendMessage;
                OnSendLocalInfoWhenOpenDataChannel(sendMessage);
                RTC_LOG(INFO) << sendMessage;
                SendDataViaDataChannel(sendMessage);
                OnSendLocalInfoWhenOpenDataChannel = nullptr;
            }
        }
    }
}

void WebrtcStreamDataChannels::OnMessage(const webrtc::DataBuffer &buffer)
{
    size_t size = buffer.data.size();
    char* msg = new char[size + 1];
    memcpy(msg, buffer.data.data(), size);
    msg[size] = 0;

    std::string recvMessage = msg;
    LOGINFO(recvMessage.c_str());

    if(OnRecvMessage){
        OnRecvMessage(msg);
    }
    delete[] msg;
}
