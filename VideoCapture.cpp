#include "VideoCapture.h"
#include <QDebug>
#include <algorithm>
#include <webrtc/api/scoped_refptr.h>
#include <webrtc/api/video/i420_buffer.h>
#include <webrtc/api/video/video_frame_buffer.h>
#include <webrtc/api/video/video_rotation.h>
#include <webrtc/rtc_base/arraysize.h>
#include <libyuv/convert_argb.h>
#include "VideoRender.h"

VideoCapture::VideoCapture(){

}

VideoCapture::~VideoCapture(){

}

void VideoCapture::AddOrUpdateSink(rtc::VideoSinkInterface<webrtc::VideoFrame> *sink, const rtc::VideoSinkWants &wants)
{
    qDebug() << "AddOrUpdateSink .................";
    broadcaster_.AddOrUpdateSink(sink, wants);
    UpdateVideoAdapter();
}

void VideoCapture::RemoveSink(rtc::VideoSinkInterface<webrtc::VideoFrame> *sink)
{
    broadcaster_.RemoveSink(sink);
    UpdateVideoAdapter();
}

void VideoCapture::OnFrame(const webrtc::VideoFrame &frame)
{
    int cropped_width = 0;
    int cropped_height = 0;
    int out_width = 0;
    int out_height = 0;
    if(!video_adapter_.AdaptFrameResolution(frame.width(), frame.height(), frame.timestamp_us() * 1000,
                                            &cropped_width, &cropped_height, &out_width, &out_height)){
        return;
    }

    rtc::scoped_refptr<webrtc::I420BufferInterface> buffer(frame.video_frame_buffer()->ToI420());
    if(frame.rotation() != webrtc::kVideoRotation_0){
        buffer = webrtc::I420Buffer::Rotate(*buffer, frame.rotation());
    }


    if(out_height != frame.height() || out_width != frame.width()){
        rtc::scoped_refptr<webrtc::I420Buffer> scaled_buffer = webrtc::I420Buffer::Create(out_width, out_height);
        scaled_buffer->ScaleFrom(*frame.video_frame_buffer()->ToI420());
        broadcaster_.OnFrame(webrtc::VideoFrame::Builder()
                             .set_video_frame_buffer(scaled_buffer)
                             .set_rotation(webrtc::kVideoRotation_0)
                             .set_timestamp_us(frame.timestamp_us())
                             .set_id(frame.id())
                             .build());
    }else{
        broadcaster_.OnFrame(frame);
    }
}

rtc::VideoSinkWants VideoCapture::GetSinkWants()
{
    return broadcaster_.wants();
}

void VideoCapture::UpdateVideoAdapter()
{
    rtc::VideoSinkWants wants = broadcaster_.wants();
    video_adapter_.OnResolutionFramerateRequest(wants.target_pixel_count, wants.max_pixel_count,
                                                wants.max_framerate_fps);
}

