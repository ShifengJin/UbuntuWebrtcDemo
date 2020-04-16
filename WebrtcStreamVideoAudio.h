#ifndef WEBRTCSTREAMVIDEOAUDIO_H
#define WEBRTCSTREAMVIDEOAUDIO_H

#include <QObject>

#include "api/media_stream_interface.h"

#include "WebrtcStreamBase.h"
#include "WebrtcVideoFrame.h"
#include "Common.h"

class WebrtcStreamVideoAudio : public WebrtcStreamBase
{
    Q_OBJECT
public:
    WebrtcStreamVideoAudio(long long mHandleID);
    ~WebrtcStreamVideoAudio();

    void SetWindowID(WINDOWID iWindowID) { mWindowID = iWindowID; }
    WINDOWID GetWindowID() { return mWindowID; }
    void SetIsLocal(bool inIsLocal) { mIsLocal = inIsLocal; }
    bool GetIsLocal() { return mIsLocal; }

    void StartView();
    void StopView();

    bool initializePeerConnection();
    void ConnectToPeer();

    bool SetPeerSDP(std::string type, std::string sdp);


    void OnAddTrack(rtc::scoped_refptr<webrtc::RtpReceiverInterface> receiver, const std::vector<rtc::scoped_refptr<webrtc::MediaStreamInterface> > &streams);
    void OnRemoveTrack(rtc::scoped_refptr<webrtc::RtpReceiverInterface> receiver);

protected:
    void addTracks();

private:

    bool mIsLocal = false;

    rtc::scoped_refptr<webrtc::VideoTrackInterface> mVideoTrack;

    WebrtcVideoFrame                               *mRender;
    WINDOWID                                        mWindowID;


};

#endif // WEBRTCSTREAMVIDEOAUDIO_H
