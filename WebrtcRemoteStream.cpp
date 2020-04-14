
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

QtWebrtcRemoteStream::QtWebrtcRemoteStream(long long peer_id)
    : mPeerID(peer_id)
{
    rtc::Thread::Current()->ProcessMessages(1);
}

bool QtWebrtcRemoteStream::CreateDataChannel()
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

void QtWebrtcRemoteStream::CloseDataChannel()
{
    if(data_channel_.get()){
        data_channel_->UnregisterObserver();
        data_channel_->Close();
    }
    data_channel_ = nullptr;
}

bool QtWebrtcRemoteStream::SendDataViaDataChannel(const std::string &data)
{
    if(!data_channel_.get()){
        return false;
    }
    webrtc::DataBuffer buffer(data);
    data_channel_->Send(buffer);
    return true;
}

void QtWebrtcRemoteStream::RegisterSendLocalSDP_CallBack(const SENDSDP_CALLBACK &callback)
{
    OnLocalSDPReady = callback;
}

void QtWebrtcRemoteStream::RegisterSendIceCandidate_callBack(SENDICECANDIDATE_CALLBACK callback)
{
    OnSendIceCandidate = callback;
}

void QtWebrtcRemoteStream::RegisterIceGatheringComplete_callBack(ICEGATHERINGCOMPLETE_CALLBACK callback)
{
    OnIceGatheringComplete = callback;
}

void QtWebrtcRemoteStream::RegisterRecvMessage_callBack(RECVMESSAGE_CALLBACK callback)
{
    OnRecvMessage = callback;
}

void QtWebrtcRemoteStream::RegisterSendLocalInfoWhenOpenDataChannel_callBack(SENDLOCALINFOWHENOPENDATACHANNEL_CALLBACK callback)
{
    OnSendLocalInfoWhenOpenDataChannel = callback;
}

QtWebrtcRemoteStream::~QtWebrtcRemoteStream()
{
    RTC_DCHECK(!mPeerConnection);
}

bool QtWebrtcRemoteStream::initializePeerConnection()
{
    if(!createPeerConnection(true))
    {
        RTC_LOG(INFO) << "CreatePeerConnection failed";
        DeletePeerConnection();
    }

    if(isLocal){
        if(CreateDataChannel()){
            RTC_LOG(INFO) << "CreateDataChannel successed.";
        }
    }else{
        addTracks();
    }
    return mPeerConnection != nullptr;
}

bool QtWebrtcRemoteStream::createPeerConnection(bool dtls)
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

void QtWebrtcRemoteStream::DeletePeerConnection()
{
    mPeerConnection = nullptr;
    mPeerID = -1;
}

void QtWebrtcRemoteStream::addTracks()
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
    auto result_or_error = mPeerConnection->AddTrack(AlvaCapturerTrackSource::GetInstall()->GetAudioTrack(),{kStreamId});
    if(!result_or_error.ok())
    {
        RTC_LOG(LS_ERROR) << "Failed to add audio track to PeerConnection:"
                          <<result_or_error.error().message();
    }

    result_or_error = mPeerConnection->AddTrack(AlvaCapturerTrackSource::GetInstall()->GetVideoTrack(),{kStreamId});
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

void QtWebrtcRemoteStream::OnAddTrack(rtc::scoped_refptr<webrtc::RtpReceiverInterface> receiver,
                                      const std::vector<rtc::scoped_refptr<webrtc::MediaStreamInterface> > &streams)
{
    RTC_LOG(INFO) << " ||| ========== OnAddTrack ========== |||";
    on_peerConnection_addTrack(receiver->track().release());
}

void QtWebrtcRemoteStream::on_peerConnection_addTrack(webrtc::MediaStreamTrackInterface* track)
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

void QtWebrtcRemoteStream::OnRemoveTrack(rtc::scoped_refptr<webrtc::RtpReceiverInterface> receiver)
{
    RTC_LOG(INFO) << " ||| ========== OnRemoveTrack ========== |||";
    if(!isLocal){
        on_peerConnection_removeTrack(receiver->track().release());
    }
}

void QtWebrtcRemoteStream::OnDataChannel(rtc::scoped_refptr<webrtc::DataChannelInterface> channel)
{
    RTC_LOG(INFO) << " ||| ========== OnDataChannel ========== |||";
    channel->RegisterObserver(this);
}

void QtWebrtcRemoteStream::on_peerConnection_removeTrack(webrtc::MediaStreamTrackInterface* track)
{
    if(track->kind() == webrtc::MediaStreamTrackInterface::kVideoKind)
    {
        auto* video_track = static_cast<webrtc::VideoTrackInterface*>(track);
        this->StopView();
    }

    track->Release();
}

void QtWebrtcRemoteStream::OnIceGatheringChange(webrtc::PeerConnectionInterface::IceGatheringState new_state)
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

void QtWebrtcRemoteStream::OnIceCandidate(const webrtc::IceCandidateInterface *candidate)
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

bool QtWebrtcRemoteStream::AddPeerIceCandidate(QString sdp_mid_, int sdp_mlineindex_, QString candidate_)
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

bool QtWebrtcRemoteStream::SetPeerSDP(std::string type, std::string sdp)
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

void QtWebrtcRemoteStream::ConnectToPeer()
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

void QtWebrtcRemoteStream::OnSuccess(webrtc::SessionDescriptionInterface *desc)
{
    RTC_LOG(INFO) << " ||| ========== OnSuccess ========== |||";
    mPeerConnection->SetLocalDescription(
                DummySetSessionDescriptionObserver::Create(),desc);

    RTC_LOG(INFO) << "  type : " << desc->type();
    std::string sdp;
    desc->ToString(&sdp);

    RTC_LOG(INFO) << "  sdp : " << sdp;
    RTC_LOG(INFO) << " ||| ========== OnSuccess ========== 11111111111  |||";

    if(OnLocalSDPReady){
        //OnLocalSDPReady(mPeerID, sdp, desc->type());
        //OnLocalSDPReady(mPeerID, STDSTR_TO_QTSTR(sdp),STDSTR_TO_QTSTR(desc->type()));
    }



    std::thread::id threadid = std::this_thread::get_id();
    std::stringstream sin;
    sin << threadid;

    RTC_LOG(INFO) << "================================> OnSuccess   thread id : " << sin.str();

    Q_EMIT LocalSDP(mPeerID, STDSTR_TO_QTSTR(sdp),STDSTR_TO_QTSTR(desc->type()));
}

void QtWebrtcRemoteStream::OnFailure(webrtc::RTCError error)
{
   RTC_LOG(INFO) << " ||| ========== OnFailure ========== |||";
   RTC_LOG(LERROR)<<ToString(error.type())<<":"<<error.message();
}

void QtWebrtcRemoteStream::OnStateChange()
{
    RTC_LOG(INFO) << " ||| ========== OnStateChange ========== |||";
    if (data_channel_) {
      webrtc::DataChannelInterface::DataState state = data_channel_->state();
      if (state == webrtc::DataChannelInterface::kOpen) {
        RTC_LOG(LS_INFO) << "Data channel is open";

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

void QtWebrtcRemoteStream::OnMessage(const webrtc::DataBuffer &buffer)
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
