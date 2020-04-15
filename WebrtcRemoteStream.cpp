
#include "WebrtcRemoteStream.h"

#include <memory>
#include <utility>
#include <vector>

#include "modules/video_capture/video_capture_factory.h"
#include "rtc_base/checks.h"
#include "rtc_base/logging.h"

#include "ConnecttionFactory.h"
#include "ConferenceManager.h"
#include "Common.h"

#include "CapturerTrackSource.h"

const char kStreamId[] = "stream_id";

class DummySetSessionDescriptionObserver
        :  public webrtc::SetSessionDescriptionObserver
{
public:
    static DummySetSessionDescriptionObserver* Create()
    {
        return new rtc::RefCountedObject<DummySetSessionDescriptionObserver>();
    }
    virtual void OnSuccess() {RTC_LOG(INFO) << __FUNCTION__;}
    virtual void OnFailure(webrtc::RTCError error)
    {
       RTC_LOG(INFO) << __FUNCTION__ << "" <<ToString(error.type()) <<":"
                     <<error.message();
    }
};

WebrtcRemoteStream::WebrtcRemoteStream(long long peer_id)
    : mPeerID(peer_id)
{
    rtc::Thread::Current()->ProcessMessages(1);
}

bool WebrtcRemoteStream::CreateDataChannel()
{
    struct webrtc::DataChannelInit init;
    init.ordered = true;
    data_channel_ = mPeerConnection->CreateDataChannel("AlvaSystems", &init);
    if(data_channel_.get()){
        data_channel_->RegisterObserver(this);
        RTC_LOG(INFO) << "CreateDataChannel successed.";
        return true;
    }
    return false;
}

void WebrtcRemoteStream::CloseDataChannel()
{
    if(data_channel_.get()){
        data_channel_->UnregisterObserver();
        data_channel_->Close();
    }
    data_channel_ = nullptr;
}

bool WebrtcRemoteStream::SendDataViaDataChannel(const std::string &data)
{
    if(!data_channel_.get()){
        return false;
    }
    webrtc::DataBuffer buffer(data);
    data_channel_->Send(buffer);
    return true;
}

void WebrtcRemoteStream::RegisterSendLocalSDP_CallBack(const SENDSDP_CALLBACK &callback)
{
    OnLocalSDPReady = callback;
}

void WebrtcRemoteStream::RegisterSendIceCandidate_callBack(SENDICECANDIDATE_CALLBACK callback)
{
    OnSendIceCandidate = callback;
}

void WebrtcRemoteStream::RegisterIceGatheringComplete_callBack(ICEGATHERINGCOMPLETE_CALLBACK callback)
{
    OnIceGatheringComplete = callback;
}

void WebrtcRemoteStream::RegisterRecvMessage_callBack(RECVMESSAGE_CALLBACK callback)
{
    OnRecvMessage = callback;
}

void WebrtcRemoteStream::RegisterSendLocalInfoWhenOpenDataChannel_callBack(SENDLOCALINFOWHENOPENDATACHANNEL_CALLBACK callback)
{
    OnSendLocalInfoWhenOpenDataChannel = callback;
}

WebrtcRemoteStream::~WebrtcRemoteStream()
{
    RTC_DCHECK(!mPeerConnection);
}

bool WebrtcRemoteStream::initializePeerConnection()
{
    if(!createPeerConnection(true))
    {
        RTC_LOG(INFO) << "CreatePeerConnection failed";
        DeletePeerConnection();
    }

    if(isLocal){
        if(CreateDataChannel()){
            qDebug() << "CreateDataChannel successed.";
        }
    }else{
        addTracks();
    }
    return mPeerConnection != nullptr;
}

bool WebrtcRemoteStream::createPeerConnection(bool dtls)
{
    webrtc::PeerConnectionInterface::RTCConfiguration config;
    config.sdp_semantics = webrtc::SdpSemantics::kUnifiedPlan;
    config.enable_dtls_srtp = dtls;

    webrtc::PeerConnectionInterface::BitrateParameters bitrateParam;
    bitrateParam.min_bitrate_bps = absl::optional<int>(128000);
    bitrateParam.current_bitrate_bps = absl::optional<int>(256000);
    bitrateParam.max_bitrate_bps = absl::optional<int>(512000);

    webrtc::PeerConnectionInterface::IceServer server;
    // 需要提供接口
    std::string serverUrl = GetPeerConnectionString();
    qDebug() << "=================================> serverUrl : " << QString::fromStdString(serverUrl);
    server.uri = serverUrl;
    config.servers.push_back(server);

    mPeerConnection = ConnecttionFactory::Get()->CreatePeerConnection(config, nullptr,nullptr,this);
    mPeerConnection->SetBitrate(bitrateParam);
    return mPeerConnection != nullptr;
}

void WebrtcRemoteStream::DeletePeerConnection()
{
    mPeerConnection = nullptr;
    mPeerID = -1;
}

void WebrtcRemoteStream::addTracks()
{
    if(!mPeerConnection->GetSenders().empty())
    {
        return;//Already added tracks.
    }
#if 0
    auto result_or_error = mPeerConnection->AddTrack(QtWebrtcLocalStream::Get()->GetAudioTrack(),{kStreamId});
    //auto result_or_error = mPeerConnection->AddTrack(CapturerTrackSource::Instance()->GetAudioTrack(),{kStreamId});
    if(!result_or_error.ok())
    {
        RTC_LOG(LS_ERROR) << "Failed to add audio track to PeerConnection:"
                          <<result_or_error.error().message();
    }

    result_or_error = mPeerConnection->AddTrack(QtWebrtcLocalStream::Get()->GetVideoTrack(),{kStreamId});
    //result_or_error = mPeerConnection->AddTrack(CapturerTrackSource::Instance()->GetVideoTrack(),{kStreamId});
    if(!result_or_error.ok())
    {
        RTC_LOG(LS_ERROR) << "Failed to add video track to PeerConnection:"
                          << result_or_error.error().message();
    }
#elif 0
    rtc::scoped_refptr<webrtc::MediaStreamInterface> stream =
          ConnecttionFactory::Get()->CreateLocalMediaStream(kStreamId);
    rtc::scoped_refptr<webrtc::AudioTrackInterface> audio_track = CapturerTrackSource::Instance()->GetAudioTrack();
    rtc::scoped_refptr<webrtc::VideoTrackInterface> video_track = CapturerTrackSource::Instance()->GetVideoTrack();
    stream->AddTrack(audio_track);
    stream->AddTrack(video_track);

    //stream->GetVideoTracks()[0]->AddOrUpdateSink(CapturerTrackSource::Instance()->GetCapturer().get(), rtc::VideoSinkWants());

    mPeerConnection->AddStream(stream);
#elif 1
    auto result_or_error = mPeerConnection->AddTrack(CapturerTrackSource::GetInstall()->GetAudioTrack(),{kStreamId});
    if(!result_or_error.ok())
    {
        RTC_LOG(LS_ERROR) << "Failed to add audio track to PeerConnection:"
                          <<result_or_error.error().message();
    }

    result_or_error = mPeerConnection->AddTrack(CapturerTrackSource::GetInstall()->GetVideoTrack(),{kStreamId});
    if(!result_or_error.ok())
    {
        RTC_LOG(LS_ERROR) << "Failed to add video track to PeerConnection:"
                          << result_or_error.error().message();
    }
#else
    rtc::scoped_refptr<webrtc::MediaStreamInterface> stream =
          ConnecttionFactory::Get()->CreateLocalMediaStream(kStreamId);
    rtc::scoped_refptr<webrtc::AudioTrackInterface> audio_track = AlvaCapturerTrackSource::GetInstall()->GetAudioTrack();
    rtc::scoped_refptr<webrtc::VideoTrackInterface> video_track = AlvaCapturerTrackSource::GetInstall()->GetVideoTrack();
    stream->AddTrack(audio_track);
    stream->AddTrack(video_track);

    //stream->GetVideoTracks()[0]->AddOrUpdateSink(CapturerTrackSource::Instance()->GetCapturer().get(), rtc::VideoSinkWants());

    mPeerConnection->AddStream(stream);
#endif
}

void WebrtcRemoteStream::OnAddTrack(rtc::scoped_refptr<webrtc::RtpReceiverInterface> receiver,
                                      const std::vector<rtc::scoped_refptr<webrtc::MediaStreamInterface> > &streams)
{
    RTC_LOG(INFO) << " ||| ========== OnAddTrack ========== |||";
    on_peerConnection_addTrack(receiver->track().release());
}

void WebrtcRemoteStream::on_peerConnection_addTrack(webrtc::MediaStreamTrackInterface* track)
{
    if(track->kind() == webrtc::MediaStreamTrackInterface::kVideoKind)
    {
        if(!isLocal){
            auto* video_track = static_cast<webrtc::VideoTrackInterface*>(track);
            this->SetVideoTrack(video_track);
            this->StartView();
        }
    }
}

void WebrtcRemoteStream::OnRemoveTrack(rtc::scoped_refptr<webrtc::RtpReceiverInterface> receiver)
{
    RTC_LOG(INFO) << " ||| ========== OnRemoveTrack ========== |||";
    if(!isLocal){
        on_peerConnection_removeTrack(receiver->track().release());
    }
}

void WebrtcRemoteStream::OnDataChannel(rtc::scoped_refptr<webrtc::DataChannelInterface> channel)
{
    RTC_LOG(INFO) << " ||| ========== OnDataChannel ========== |||";
    channel->RegisterObserver(this);
}

void WebrtcRemoteStream::on_peerConnection_removeTrack(webrtc::MediaStreamTrackInterface* track)
{
    if(track->kind() == webrtc::MediaStreamTrackInterface::kVideoKind)
    {
        auto* video_track = static_cast<webrtc::VideoTrackInterface*>(track);
        this->StopView();
    }

    track->Release();
}

void WebrtcRemoteStream::OnIceGatheringChange(webrtc::PeerConnectionInterface::IceGatheringState new_state)
{
    RTC_LOG(INFO) << " ||| ========== OnIceGatheringChange ========== |||";
#if 1
    switch(new_state)
    {
       case webrtc::PeerConnectionInterface::kIceGatheringNew:
            RTC_LOG(LERROR) << "---------------------" <<"kIceGatheringNew";
            break;
       case webrtc::PeerConnectionInterface::kIceGatheringGathering:
            RTC_LOG(LERROR) <<"-----------------------" << "kIceGatheringGathering";
            break;
       case webrtc::PeerConnectionInterface::kIceGatheringComplete:
            RTC_LOG(LERROR) <<"---------------------" << "kIceGatheringComplete";
            break;
    default:
        break;
    }

    if(new_state == webrtc::PeerConnectionInterface::kIceGatheringComplete)
    {
        Q_EMIT IceGatheringComplete(mPeerID);
    }
#endif
}

void WebrtcRemoteStream::OnIceCandidate(const webrtc::IceCandidateInterface *candidate)
{
    RTC_LOG(INFO) << " ||| ========== OnIceCandidate ========== |||";
    std::string sdp;
    if(!candidate->ToString(&sdp))
    {
        RTC_LOG(LS_ERROR) << "Failed to serialize candidate";
        return;
    }
    Q_EMIT LocalIceCandidate(mPeerID,STDSTR_TO_QTSTR(candidate->sdp_mid()),candidate->sdp_mline_index(),STDSTR_TO_QTSTR(sdp));
}

bool WebrtcRemoteStream::AddPeerIceCandidate(QString sdp_mid_, int sdp_mlineindex_, QString candidate_)
{
    RTC_DCHECK(!sdp_mid_.isEmpty());
    RTC_DCHECK(!candidate_.isEmpty());

    if(!mPeerConnection.get())
    {
        if(!initializePeerConnection())
        {
              RTC_LOG(LS_ERROR) << "Failed to initialize our PeerConnection instance";
              return false;
        }
    }

    webrtc::SdpParseError error;
    std::unique_ptr<webrtc::IceCandidateInterface> candidate(
                webrtc::CreateIceCandidate(QTSTR_TO_STDSTR(sdp_mid_),sdp_mlineindex_,QTSTR_TO_STDSTR(candidate_),&error));
    if(!candidate.get())
    {
          RTC_LOG(WARNING) << "Can't parse received candidate message."
            <<"SdpParseError was:" << error.description;
          return false;
    }
    if(!mPeerConnection->AddIceCandidate(candidate.get()))
    {
         RTC_LOG(WARNING) << "Failed to apply the received candidate";
         return false;
    }

    return true;
}

bool WebrtcRemoteStream::SetPeerSDP(std::string type, std::string sdp)
{
    RTC_DCHECK(!sdp.empty());
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
    RTC_LOG(INFO) << __FUNCTION__ << "  type : " << type;
    if(!session_description)
    {
        RTC_LOG(WARNING) << "Can't parse received session description message."
                               <<"SdpParseError was:"<<error.description;
        return false;
    }
    RTC_LOG(INFO) << __FUNCTION__ << "  type : " << type;
    mPeerConnection->SetRemoteDescription(
                        DummySetSessionDescriptionObserver::Create(), session_description);

    if(session_description->type() ==
                        webrtc::SessionDescriptionInterface::kOffer)
    {
         qDebug() << "\n CreateAnswer start ================== mPeerID : " << mPeerID;
         mPeerConnection->CreateAnswer(this, webrtc::PeerConnectionInterface::RTCOfferAnswerOptions(false, false, true, false, true));
         qDebug() << "\n CreateAnswer end ===================\n";
    }

    return true;
}

void WebrtcRemoteStream::ConnectToPeer()
{
   RTC_DCHECK(mPeerID != -1);

   if(mPeerConnection.get())
   {
       RTC_LOG(INFO) << "We only support connecting to one peer at a time";
       return;
   }

   if(initializePeerConnection())
   {
       qDebug() << "\n CreateOffer start ================== mPeerID : " << mPeerID;
       mPeerConnection->CreateOffer(this, webrtc::PeerConnectionInterface::RTCOfferAnswerOptions());
       qDebug() << "\n CreateOffer end ===================\n";
   }
   else
   {
       RTC_LOG(INFO) << "Failed to initialize PeerConnection";
   }
}

void WebrtcRemoteStream::OnSuccess(webrtc::SessionDescriptionInterface *desc)
{
    qDebug() << " ||| ========== OnSuccess ========== |||";
    mPeerConnection->SetLocalDescription(
                DummySetSessionDescriptionObserver::Create(),desc);

    std::string sdp;
    desc->ToString(&sdp);

    Q_EMIT LocalSDP(mPeerID, STDSTR_TO_QTSTR(sdp),STDSTR_TO_QTSTR(desc->type()));
}

void WebrtcRemoteStream::OnFailure(webrtc::RTCError error)
{
   qDebug() << " ||| ========== OnFailure ========== |||";
   RTC_LOG(LERROR)<<ToString(error.type())<<":"<<error.message();
}

void WebrtcRemoteStream::OnStateChange()
{
    qDebug() << " ||| ========== OnStateChange ========== |||";
    if (data_channel_) {
      webrtc::DataChannelInterface::DataState state = data_channel_->state();
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

void WebrtcRemoteStream::OnMessage(const webrtc::DataBuffer &buffer)
{
    RTC_LOG(INFO) << " ||| ========== OnMessage ========== |||";
    size_t size = buffer.data.size();
    char* msg = new char[size + 1];
    memcpy(msg, buffer.data.data(), size);
    msg[size] = 0;

    std::string recvMessage = msg;
    RTC_LOG(INFO) << recvMessage;

    if(OnRecvMessage){
        OnRecvMessage(msg);
    }
    delete[] msg;
}
