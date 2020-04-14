#ifndef QTWEBRTCREMOTESTREAM_H
#define QTWEBRTCREMOTESTREAM_H



#include <deque>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include <QObject>
#include "WebrtcStream.h"

#include "api/media_stream_interface.h"
#include "api/peer_connection_interface.h"

typedef std::function<void(long long, std::string, std::string)> SENDSDP_CALLBACK;
typedef std::function<void(long long, std::string, int, std::string)> SENDICECANDIDATE_CALLBACK;
typedef std::function<void(long long)> ICEGATHERINGCOMPLETE_CALLBACK;
typedef std::function<void(std::string)> RECVMESSAGE_CALLBACK;
typedef std::function<void(std::string&)> SENDLOCALINFOWHENOPENDATACHANNEL_CALLBACK;

class QtWebrtcRemoteStream : public QObject, public QtWebrtcStream,
    public webrtc::PeerConnectionObserver, public webrtc::CreateSessionDescriptionObserver, public webrtc::DataChannelObserver
{
    Q_OBJECT
public:
    QtWebrtcRemoteStream(long long peer_id);

    long long id() const { return mPeerID; }
    void setid(long long id) { mPeerID = id; }

    bool CreateDataChannel();
    void CloseDataChannel();
    bool SendDataViaDataChannel(const std::string& data);

    void SetIsLocal(bool inIsLocal) {isLocal = inIsLocal;}
    bool GetIsLocal(){return isLocal;}

    void RegisterSendLocalSDP_CallBack(const SENDSDP_CALLBACK &callback);
    void RegisterSendIceCandidate_callBack(SENDICECANDIDATE_CALLBACK callback);
    void RegisterIceGatheringComplete_callBack(ICEGATHERINGCOMPLETE_CALLBACK callback);
    void RegisterRecvMessage_callBack(RECVMESSAGE_CALLBACK callback);
    void RegisterSendLocalInfoWhenOpenDataChannel_callBack(SENDLOCALINFOWHENOPENDATACHANNEL_CALLBACK callback);

public Q_SLOTS:

    void ConnectToPeer();
    bool AddPeerIceCandidate(QString sdp_mid, int sdp_mlineindex, QString candidate);  // 不会调用
    bool SetPeerSDP(std::string type, std::string sdp);
    void DeletePeerConnection();

Q_SIGNALS:
    void LocalIceCandidate(long long id, QString sdp_mid, int sdp_mlineindex, QString candidate);

    void LocalSDP(long long id, QString sdp, QString type);
    void IceGatheringComplete(long long id);

protected:
    ~QtWebrtcRemoteStream();
    bool initializePeerConnection();
    bool createPeerConnection(bool dtls);
    void addTracks();

public:

    void OnSignalingChange(webrtc::PeerConnectionInterface::SignalingState new_state) override {}
    void OnAddTrack(rtc::scoped_refptr<webrtc::RtpReceiverInterface> receiver,
        const std::vector<rtc::scoped_refptr<webrtc::MediaStreamInterface>>& streams) override;
    void OnRemoveTrack(rtc::scoped_refptr<webrtc::RtpReceiverInterface> receiver) override;
    void OnDataChannel(rtc::scoped_refptr<webrtc::DataChannelInterface> channel) override;
    void OnRenegotiationNeeded() override {}
    void OnIceConnectionChange(webrtc::PeerConnectionInterface::IceConnectionState new_state) override {}
    void OnIceGatheringChange(webrtc::PeerConnectionInterface::IceGatheringState new_state) override;
    void OnIceCandidate(const webrtc::IceCandidateInterface* candidate) override;
    void OnIceConnectionReceivingChange(bool receiving) override {}

    void OnSuccess(webrtc::SessionDescriptionInterface* desc) override;
    void OnFailure(webrtc::RTCError error) override;

    // The data channel state have changed.
    void OnStateChange();
    //  A data buffer was successfully received.
    void OnMessage(const webrtc::DataBuffer& buffer);

private Q_SLOTS:
    void on_peerConnection_addTrack(webrtc::MediaStreamTrackInterface* track);
    void on_peerConnection_removeTrack(webrtc::MediaStreamTrackInterface* track);

public:
    SENDSDP_CALLBACK              OnLocalSDPReady = nullptr;
    SENDICECANDIDATE_CALLBACK     OnSendIceCandidate = nullptr;
    ICEGATHERINGCOMPLETE_CALLBACK OnIceGatheringComplete = nullptr;
    RECVMESSAGE_CALLBACK          OnRecvMessage = nullptr;
    SENDLOCALINFOWHENOPENDATACHANNEL_CALLBACK    OnSendLocalInfoWhenOpenDataChannel;

protected:
    bool isLocal = false;
    long long mPeerID = -1;
    rtc::scoped_refptr<webrtc::PeerConnectionInterface> mPeerConnection;
    rtc::scoped_refptr<webrtc::DataChannelInterface> data_channel_;
};
#endif // QTWEBRTCREMOTESTREAM_H
