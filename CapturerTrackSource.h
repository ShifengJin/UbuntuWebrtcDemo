#ifndef CAAPTURERTRACKSOURCE_H
#define CAAPTURERTRACKSOURCE_H

#include <utility>
#include <webrtc/absl/memory/memory.h>
#include <webrtc/pc/video_track_source.h>
#include "VcmCapture.h"
#include "WebrtcVideoFrame.h"

class CapturerTrackSource : public webrtc::VideoTrackSource {
 public:
  CapturerTrackSource();

  VcmCapture* GetCapturer(){return capturer_;}

private:
  rtc::VideoSourceInterface<webrtc::VideoFrame>* source() override {
    return capturer_;
  }
  VcmCapture* capturer_;
};

class AlvaCapturerTrackSource
{
public:

    AlvaCapturerTrackSource();

    void CreateVideoTrackAndAudioTrack();

    rtc::scoped_refptr<webrtc::VideoTrackInterface> GetVideoTrack();

    rtc::scoped_refptr<webrtc::AudioTrackInterface> GetAudioTrack();

    static AlvaCapturerTrackSource* GetInstall();

    rtc::scoped_refptr<CapturerTrackSource> getCapturerTrackSource(){return mCapturerTrackSource;}

private:

    rtc::scoped_refptr<webrtc::VideoTrackInterface> mVideoTrack;
    rtc::scoped_refptr<webrtc::AudioTrackInterface> mAudioTrack;
    rtc::scoped_refptr<CapturerTrackSource>       mCapturerTrackSource;

    static AlvaCapturerTrackSource* install;

};

#endif // CAAPTURERTRACKSOURCE_H
