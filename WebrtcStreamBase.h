#ifndef WEBRTCSTREAMBASE_H
#define WEBRTCSTREAMBASE_H

#include <QObject>

#include "api/peer_connection_interface.h"
#include "Common.h"

class DummySetSessionDescriptionObserver
        :  public webrtc::SetSessionDescriptionObserver
{
public:
    static DummySetSessionDescriptionObserver* Create()
    {
        return new rtc::RefCountedObject<DummySetSessionDescriptionObserver>();
    }
    virtual void OnSuccess() {LOGINFO("");}
    virtual void OnFailure(webrtc::RTCError error)
    {
       LOGINFO(error.message());
    }
};

class WebrtcStreamBase : public QObject, public webrtc::PeerConnectionObserver, public webrtc::CreateSessionDescriptionObserver
{
    Q_OBJECT
public:
    WebrtcStreamBase(long long inHandleID);
    ~WebrtcStreamBase();

    long long GetHandleID() { return mHandleID; }

public:
    void OnSignalingChange(webrtc::PeerConnectionInterface::SignalingState new_state);

    void OnAddStream(rtc::scoped_refptr<webrtc::MediaStreamInterface> stream);

    void OnRemoveStream(rtc::scoped_refptr<webrtc::MediaStreamInterface> stream);

    void OnDataChannel(rtc::scoped_refptr<webrtc::DataChannelInterface> data_channel);

    void OnRenegotiationNeeded();

    void OnIceConnectionChange(webrtc::PeerConnectionInterface::IceConnectionState new_state);

    void OnStandardizedIceConnectionChange(webrtc::PeerConnectionInterface::IceConnectionState new_state);

    void OnConnectionChange(webrtc::PeerConnectionInterface::PeerConnectionState new_state);

    // Called any time the IceGatheringState changes.
    void OnIceGatheringChange(webrtc::PeerConnectionInterface::IceGatheringState new_state);

    // A new ICE candidate has been gathered.
    void OnIceCandidate(const webrtc::IceCandidateInterface* candidate);

    // Ice candidates have been removed.
    // TODO(honghaiz): Make this a pure virtual method when all its subclasses
    // implement it.
    void OnIceCandidatesRemoved(const std::vector<cricket::Candidate>& candidates);

    // Called when the ICE connection receiving status changes.
    void OnIceConnectionReceivingChange(bool receiving);

    // This is called when a receiver and its track are created.
    // TODO(zhihuang): Make this pure virtual when all subclasses implement it.
    // Note: This is called with both Plan B and Unified Plan semantics. Unified
    // Plan users should prefer OnTrack, OnAddTrack is only called as backwards
    // compatibility (and is called in the exact same situations as OnTrack).
    void OnAddTrack(rtc::scoped_refptr<webrtc::RtpReceiverInterface> receiver,
                    const std::vector<rtc::scoped_refptr<webrtc::MediaStreamInterface>>& streams);

    // This is called when signaling indicates a transceiver will be receiving
    // media from the remote endpoint. This is fired during a call to
    // SetRemoteDescription. The receiving track can be accessed by:
    // |transceiver->receiver()->track()| and its associated streams by
    // |transceiver->receiver()->streams()|.
    // Note: This will only be called if Unified Plan semantics are specified.
    // This behavior is specified in section 2.2.8.2.5 of the "Set the
    // RTCSessionDescription" algorithm:
    // https://w3c.github.io/webrtc-pc/#set-description
    void OnTrack(rtc::scoped_refptr<webrtc::RtpTransceiverInterface> transceiver);

    // Called when signaling indicates that media will no longer be received on a
    // track.
    // With Plan B semantics, the given receiver will have been removed from the
    // PeerConnection and the track muted.
    // With Unified Plan semantics, the receiver will remain but the transceiver
    // will have changed direction to either sendonly or inactive.
    // https://w3c.github.io/webrtc-pc/#process-remote-track-removal
    // TODO(hbos,deadbeef): Make pure virtual when all subclasses implement it.
    void OnRemoveTrack(rtc::scoped_refptr<webrtc::RtpReceiverInterface> receiver);

    // Called when an interesting usage is detected by WebRTC.
    // An appropriate action is to add information about the context of the
    // PeerConnection and write the event to some kind of "interesting events"
    // log function.
    // The heuristics for defining what constitutes "interesting" are
    // implementation-defined.
    void OnInterestingUsage(int usage_pattern);



    // This callback transfers the ownership of the |desc|.
    // TODO(deadbeef): Make this take an std::unique_ptr<> to avoid confusion
    // around ownership.
    void OnSuccess(webrtc::SessionDescriptionInterface* desc);
    // The OnFailure callback takes an RTCError, which consists of an
    // error code and a string.
    // RTCError is non-copyable, so it must be passed using std::move.
    // Earlier versions of the API used a string argument. This version
    // is deprecated; in order to let clients remove the old version, it has a
    // default implementation. If both versions are unimplemented, the
    // result will be a runtime error (stack overflow). This is intentional.
    void OnFailure(const std::string& error);


Q_SIGNALS:
    void LocalIceCandidate(long long handleID, QString sdp_mid, int sdp_mlineindex, QString candidate);
    void LocalSDP(long long handleID, QString sdp, QString type);
    void IceGatheringComplete(long long handleID);

public Q_SLOTS:
    void DeletePeerConnection();

protected:

    bool createPeerConnection(bool dtls);


protected:
    long long mHandleID = 0;

    rtc::scoped_refptr<webrtc::PeerConnectionInterface> mPeerConnection;
};

#endif // WEBRTCSTREAMBASE_H
