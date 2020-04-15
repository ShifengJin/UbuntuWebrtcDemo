#ifndef TESTVIDEOCAPTURE_H
#define TESTVIDEOCAPTURE_H

#include <stddef.h>
#include <memory>
#include <webrtc/api/video/video_frame.h>
#include <webrtc/api/video/video_source_interface.h>
#include <webrtc/media/base/video_adapter.h>
#include <webrtc/media/base/video_broadcaster.h>
#include <webrtc/api/media_stream_interface.h>

#include "Common.h"

class VideoCapture : public rtc::VideoSourceInterface<webrtc::VideoFrame>
{
public:
    VideoCapture();
    ~VideoCapture() override;

    void AddOrUpdateSink(rtc::VideoSinkInterface<webrtc::VideoFrame> * sink,
                         const rtc::VideoSinkWants& wants) override;

    void RemoveSink(rtc::VideoSinkInterface<webrtc::VideoFrame>* sink) override;

protected:
    void OnFrame(const webrtc::VideoFrame& frame);
    rtc::VideoSinkWants GetSinkWants();

private:
    void UpdateVideoAdapter();
    rtc::VideoBroadcaster broadcaster_;
    cricket::VideoAdapter video_adapter_;

};

#endif // TESTVIDEOCAPTURE_H
