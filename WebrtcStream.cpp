#include "WebrtcStream.h"
#include <webrtc/rtc_base/arraysize.h>
#include <QDebug>

WebrtcStream::WebrtcStream()
{

}

WebrtcStream::~WebrtcStream()
{
    this->StopView();
    mAudioTrack = NULL;
    mVideoTrack = NULL;
    mRender = NULL;
    mWindowID = -1;
}

rtc::scoped_refptr<webrtc::VideoTrackInterface> WebrtcStream::GetVideoTrack()
{
    return mVideoTrack;
}

rtc::scoped_refptr<webrtc::AudioTrackInterface> WebrtcStream::GetAudioTrack()
{
    return mAudioTrack;
}

void WebrtcStream::SetWindowID(unsigned long iWindowID)
{
    this->mWindowID = iWindowID;
}

unsigned long WebrtcStream::GetWindowID()
{
    return this->mWindowID;
}

void WebrtcStream::SetVideoTrack(rtc::scoped_refptr<webrtc::VideoTrackInterface> iVideoTrack)
{
    this->mVideoTrack = iVideoTrack;
}

void WebrtcStream::StartView()
{
    if(mVideoTrack != NULL){
        mRender = new WebrtcVideoFrame(this->mWindowID);
        mVideoTrack->AddOrUpdateSink((rtc::VideoSinkInterface<webrtc::VideoFrame>*)mRender, rtc::VideoSinkWants());
    }
}

void WebrtcStream::StopView()
{
    if(mVideoTrack != NULL){
        mVideoTrack->RemoveSink((rtc::VideoSinkInterface<webrtc::VideoFrame>*)mRender);
        mVideoTrack = NULL;
    }
    mRender = NULL;
}
