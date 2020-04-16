#include "CapturerTrackSource.h"

#include "WebrtcStreamVideoAudio.h"


const char kStreamId[] = "stream_id";

WebrtcStreamVideoAudio::WebrtcStreamVideoAudio(long long mHandleID) : WebrtcStreamBase(mHandleID)
{

}

WebrtcStreamVideoAudio::~WebrtcStreamVideoAudio()
{
    this->StopView();
    mVideoTrack = NULL;
    if(mRender){
        delete mRender;
    }
    mRender = NULL;
    mWindowID = -1;
}

void WebrtcStreamVideoAudio::StartView()
{
    if(mVideoTrack != NULL){
        mRender = new WebrtcVideoFrame(this->mWindowID);
        mVideoTrack->AddOrUpdateSink((rtc::VideoSinkInterface<webrtc::VideoFrame>*)mRender, rtc::VideoSinkWants());
    }
}

void WebrtcStreamVideoAudio::StopView()
{
    if(mVideoTrack != NULL){
        mVideoTrack->RemoveSink((rtc::VideoSinkInterface<webrtc::VideoFrame>*)mRender);
        mVideoTrack = NULL;
        mRender->SetWindowID(-1);
    }
}

bool WebrtcStreamVideoAudio::initializePeerConnection()
{
    if(!createPeerConnection(true)){
        LOGINFO("CreatePeerConnection failed.");
        DeletePeerConnection();
    }

    addTracks();
    return mPeerConnection != nullptr;
}

void WebrtcStreamVideoAudio::ConnectToPeer()
{
    if(mPeerConnection.get()){
        LOGINFO("We only support connectting to one peer at a time.");
        return;
    }

    if(initializePeerConnection()){

        mPeerConnection->CreateOffer(this, webrtc::PeerConnectionInterface::RTCOfferAnswerOptions());
    }
}

bool WebrtcStreamVideoAudio::SetPeerSDP(std::string type, std::string sdp)
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

    mPeerConnection->SetRemoteDescription(DummySetSessionDescriptionObserver::Create(), session_description);

    if(session_description->type() == webrtc::SessionDescriptionInterface::kOffer)
    {
        mPeerConnection->CreateAnswer(this, webrtc::PeerConnectionInterface::RTCOfferAnswerOptions(false, false, true, false, true));
    }

    return true;
}

void WebrtcStreamVideoAudio::OnAddTrack(rtc::scoped_refptr<webrtc::RtpReceiverInterface> receiver, const std::vector<rtc::scoped_refptr<webrtc::MediaStreamInterface> > &streams)
{
    if(receiver->track().release()->kind() == webrtc::MediaStreamTrackInterface::kVideoKind)
    {
        if(!mIsLocal){
            auto* video_track = static_cast<webrtc::VideoTrackInterface*>(receiver->track().release());
            mVideoTrack = video_track;
            this->StartView();
        }
    }
}

void WebrtcStreamVideoAudio::OnRemoveTrack(rtc::scoped_refptr<webrtc::RtpReceiverInterface> receiver)
{
    if(!mIsLocal){
        if(receiver->track().release()->kind() == webrtc::MediaStreamTrackInterface::kVideoKind)
        {
            auto* video_track = static_cast<webrtc::VideoTrackInterface*>(receiver->track().release());
            this->StopView();
        }
        receiver->track().release()->Release();
    }
}

void WebrtcStreamVideoAudio::addTracks()
{
    if(!mPeerConnection->GetSenders().empty())
    {
        return;//Already added tracks.
    }
    auto result_or_error = mPeerConnection->AddTrack(CapturerTrackSource::GetInstall()->GetAudioTrack(), { kStreamId });
    if(!result_or_error.ok())
    {
        RTC_LOG(LS_ERROR) << "Failed to add audio track to PeerConnection:"
                          <<result_or_error.error().message();
    }

    result_or_error = mPeerConnection->AddTrack(CapturerTrackSource::GetInstall()->GetVideoTrack(), { kStreamId });
    if(!result_or_error.ok())
    {
        RTC_LOG(LS_ERROR) << "Failed to add video track to PeerConnection:"
                          << result_or_error.error().message();
    }
}
