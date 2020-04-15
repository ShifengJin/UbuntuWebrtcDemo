#ifndef CAPTURERTRACKSOURCE_H
#define CAPTURERTRACKSOURCE_H

#include <utility>
#include <webrtc/absl/memory/memory.h>
#include <webrtc/pc/video_track_source.h>
#include "VcmCapture.h"
#include "WebrtcVideoFrame.h"

class CapturerTrackSourceBase : public webrtc::VideoTrackSource {
 public:
  CapturerTrackSourceBase();

  VcmCapture* GetCapturer(){return capturer_;}

private:
  rtc::VideoSourceInterface<webrtc::VideoFrame>* source() override {
    return capturer_;
  }
  VcmCapture* capturer_;
};

class CapturerTrackSource
{
public:

    CapturerTrackSource();

    void CreateVideoTrackAndAudioTrack();

    rtc::scoped_refptr<webrtc::VideoTrackInterface> GetVideoTrack();

    rtc::scoped_refptr<webrtc::AudioTrackInterface> GetAudioTrack();

    static CapturerTrackSource* GetInstall();

    rtc::scoped_refptr<CapturerTrackSourceBase> getCapturerTrackSource(){return mCapturerTrackSource;}

private:

    rtc::scoped_refptr<webrtc::VideoTrackInterface> mVideoTrack;
    rtc::scoped_refptr<webrtc::AudioTrackInterface> mAudioTrack;
    rtc::scoped_refptr<CapturerTrackSourceBase>       mCapturerTrackSource;

    static CapturerTrackSource* install;

};

#endif // CAPTURERTRACKSOURCE_H
