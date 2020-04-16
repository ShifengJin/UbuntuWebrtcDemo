#include <QDebug>

#include "api/create_peerconnection_factory.h"
#include "modules/video_capture/video_capture_factory.h"
#include "api/media_stream_interface.h"
#include "api/peer_connection_interface.h"

#include "ConnecttionFactory.h"

#include "WebrtcStreamBase.h"

#include "Common.h"


WebrtcStreamBase::WebrtcStreamBase(long long inHandleID)
{
    mHandleID = inHandleID;
}

WebrtcStreamBase::~WebrtcStreamBase()
{
    mPeerConnection = nullptr;
    mHandleID = -1;
}

void WebrtcStreamBase::OnSignalingChange(webrtc::PeerConnectionInterface::SignalingState new_state)
{
    LOGINFO("");
}

void WebrtcStreamBase::OnAddStream(rtc::scoped_refptr<webrtc::MediaStreamInterface> stream)
{
    LOGINFO("");
}

void WebrtcStreamBase::OnRemoveStream(rtc::scoped_refptr<webrtc::MediaStreamInterface> stream)
{
    LOGINFO("");
}

void WebrtcStreamBase::OnDataChannel(rtc::scoped_refptr<webrtc::DataChannelInterface> data_channel)
{
    // data_channel->RegisterObserver(this);  /// 数据通道继承时需要注册
    LOGINFO("");
}

void WebrtcStreamBase::OnRenegotiationNeeded()
{
    LOGINFO("");
}

void WebrtcStreamBase::OnIceConnectionChange(webrtc::PeerConnectionInterface::IceConnectionState new_state)
{
    LOGINFO("");
}

void WebrtcStreamBase::OnStandardizedIceConnectionChange(webrtc::PeerConnectionInterface::IceConnectionState new_state)
{
    LOGINFO("");
}

void WebrtcStreamBase::OnConnectionChange(webrtc::PeerConnectionInterface::PeerConnectionState new_state)
{
    LOGINFO("");
}

void WebrtcStreamBase::OnIceGatheringChange(webrtc::PeerConnectionInterface::IceGatheringState new_state)
{
    LOGINFO("");
    switch (new_state) {
    case webrtc::PeerConnectionInterface::kIceGatheringNew:
        break;
    case webrtc::PeerConnectionInterface::kIceGatheringGathering:
        break;
    case webrtc::PeerConnectionInterface::kIceGatheringComplete:
        Q_EMIT IceGatheringComplete(mHandleID);
        break;
    default:
        break;
    }
}

void WebrtcStreamBase::OnIceCandidate(const webrtc::IceCandidateInterface *candidate)
{
    LOGINFO("");
    std::string sdp;
    if(!candidate->ToString(&sdp)){
        LOGINFO("Failed to serialize candidate");
        return;
    }
    Q_EMIT LocalIceCandidate(mHandleID, STDSTR_TO_QTSTR(candidate->sdp_mid()), candidate->sdp_mline_index(), STDSTR_TO_QTSTR(sdp));
}

void WebrtcStreamBase::OnIceCandidatesRemoved(const std::vector<cricket::Candidate> &candidates)
{
    LOGINFO("");
}

void WebrtcStreamBase::OnIceConnectionReceivingChange(bool receiving)
{
    LOGINFO("");
}

void WebrtcStreamBase::OnAddTrack(rtc::scoped_refptr<webrtc::RtpReceiverInterface> receiver, const std::vector<rtc::scoped_refptr<webrtc::MediaStreamInterface> > &streams)
{
    LOGINFO("");
}

void WebrtcStreamBase::OnTrack(rtc::scoped_refptr<webrtc::RtpTransceiverInterface> transceiver)
{
    LOGINFO("");
}

void WebrtcStreamBase::OnRemoveTrack(rtc::scoped_refptr<webrtc::RtpReceiverInterface> receiver)
{
    LOGINFO("");
    /// 音视频调用
}

void WebrtcStreamBase::OnInterestingUsage(int usage_pattern)
{
    LOGINFO("");
}

void WebrtcStreamBase::OnSuccess(webrtc::SessionDescriptionInterface *desc)
{
    mPeerConnection->SetLocalDescription(DummySetSessionDescriptionObserver::Create(), desc);

    std::string sdp;
    desc->ToString(&sdp);

    Q_EMIT LocalSDP(mHandleID, STDSTR_TO_QTSTR(sdp), STDSTR_TO_QTSTR(desc->type()));
}

void WebrtcStreamBase::OnFailure(const std::string &error)
{
    LOGINFO(error.c_str());
}

void WebrtcStreamBase::DeletePeerConnection()
{
    mPeerConnection = nullptr;
    mHandleID = -1;
}

bool WebrtcStreamBase::createPeerConnection(bool dtls)
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
    // server.uri = GetServerUrl();
    server.uri = "stun:192.168.1.104";
    config.servers.push_back(server);

    mPeerConnection = ConnecttionFactory::Get()->CreatePeerConnection(config, nullptr, nullptr, this);

    mPeerConnection->SetBitrate(bitrateParam);

    return mPeerConnection != nullptr;
}
