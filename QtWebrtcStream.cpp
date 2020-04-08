#include "QtWebrtcStream.h"
#include <webrtc/rtc_base/arraysize.h>
#include <QDebug>

QtWebrtcStream::QtWebrtcStream()
{

}

QtWebrtcStream::~QtWebrtcStream()
{
    this->StopView();
    mAudioTrack = NULL;
    mVideoTrack = NULL;
    mRender = NULL;
    mWindowID = -1;
}

rtc::scoped_refptr<webrtc::VideoTrackInterface> QtWebrtcStream::GetVideoTrack()
{
    return mVideoTrack;
}

rtc::scoped_refptr<webrtc::AudioTrackInterface> QtWebrtcStream::GetAudioTrack()
{
    return mAudioTrack;
}

void QtWebrtcStream::SetWindowID(unsigned long iWindowID)
{
    this->mWindowID = iWindowID;
}

unsigned long QtWebrtcStream::GetWindowID()
{
    return this->mWindowID;
}

void QtWebrtcStream::SetVideoTrack(rtc::scoped_refptr<webrtc::VideoTrackInterface> iVideoTrack)
{
    this->mVideoTrack = iVideoTrack;
}

void QtWebrtcStream::StartView()
{
    if(mVideoTrack != NULL){
        mRender = new QtWebRTCVideoFrame(this->mWindowID);
        mVideoTrack->AddOrUpdateSink((rtc::VideoSinkInterface<webrtc::VideoFrame>*)mRender, rtc::VideoSinkWants());
    }
}

void QtWebrtcStream::StopView()
{
    if(mVideoTrack != NULL){
        mVideoTrack->RemoveSink((rtc::VideoSinkInterface<webrtc::VideoFrame>*)mRender);
        mVideoTrack = NULL;
    }
    mRender = NULL;
}
