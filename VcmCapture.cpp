#include <QDebug>
#include "VcmCapture.h"
#include <webrtc/modules/video_capture/video_capture_factory.h>
#include <webrtc/rtc_base/checks.h>
#include <webrtc/rtc_base/logging.h>
#include "ConnecttionFactory.h"
#include <libyuv/convert_argb.h>
#include <webrtc/api/video/i420_buffer.h>

#include <webrtc/video/video_send_stream.h>
#include <webrtc/video/video_stream_decoder.h>
#include <webrtc/video/video_stream_encoder.h>

#include "VideoRender.h"

VcmCapture::~VcmCapture()
{
    Destroy();
}

void VcmCapture::OnFrame(const webrtc::VideoFrame &frame)
{

    // qDebug() << "============================> OnFrame";
    // 此处可以作为回调函数使用
    // ------------------------------------------------
    rtc::scoped_refptr<webrtc::I420BufferInterface> buffer(frame.video_frame_buffer()->ToI420());
    if(frame.rotation() != webrtc::kVideoRotation_0){
        buffer = webrtc::I420Buffer::Rotate(*buffer, frame.rotation());
    }

    VideoRender *paintWidget = (VideoRender*)QWidget::find(mWindowID);

    if(paintWidget == nullptr)
    {
       qDebug() << "paintWidget nullptr,ERROR ,return";
       return;
    }

    int yuvBufferSize = buffer->width() * buffer->height() * 3 / 2;
    unsigned char * yuvBuffer = (unsigned char*)malloc(yuvBufferSize);

    RTC_CHECK(yuvBuffer!=nullptr);

    memcpy(yuvBuffer, buffer->DataY(), buffer->width() * buffer->height());
    memcpy(yuvBuffer + buffer->width() * buffer->height(), buffer->DataU(), buffer->width() * buffer->height() / 4);
    memcpy(yuvBuffer + buffer->width() * buffer->height() + buffer->width() * buffer->height() / 4, buffer->DataV(), buffer->width() * buffer->height() / 4);
    paintWidget->SetVideoSize(buffer->width(), buffer->height());
    paintWidget->SetYUVData(yuvBuffer);

    paintWidget->SetViewStated(true);
    if(yuvBuffer != nullptr){
        free(yuvBuffer);
        yuvBuffer = nullptr;
    }

    // ------------------------------------------------
    VideoCapture::OnFrame(frame);
}
#if ALVA_WEBRTC_USE_WEBRTCCAPTURE

#else
void VcmCapture::SendFrame(int width, int height, const uint8_t *data_y, int stride_y, const uint8_t *data_u, int stride_u, const uint8_t *data_v, int stride_v, uint16_t frame_id)
{
    rtc::scoped_refptr<webrtc::I420Buffer> buffer = webrtc::I420Buffer::Copy(width, height, data_y, stride_y, data_u, stride_u, data_v, stride_v);
    int64_t timestamp_us = rtc::TimeMicros() * 1000;
    webrtc::VideoFrame SelfCaptureVideoFrame = webrtc::VideoFrame::Builder().set_video_frame_buffer(buffer)
                                                                            .set_rotation(webrtc::kVideoRotation_0)
                                                                            .set_timestamp_us(timestamp_us)
                                                                            .set_id(frame_id)
                                                                            .build();

    OnFrame(SelfCaptureVideoFrame);
}
#endif
VcmCapture::VcmCapture() : vcm_(nullptr)
{

}

VcmCapture::VcmCapture(size_t width, size_t height, size_t target_fps, size_t capture_device_index)
{
    Init(width, height, target_fps, capture_device_index);
}

bool VcmCapture::Init(size_t width, size_t height, size_t target_fps, size_t capture_device_index)
{
#if ALVA_WEBRTC_USE_WEBRTCCAPTURE
    std::unique_ptr<webrtc::VideoCaptureModule::DeviceInfo> device_info(
                webrtc::VideoCaptureFactory::CreateDeviceInfo());

    char device_name[256];
    char unique_name[256];
    if(device_info->GetDeviceName(static_cast<uint32_t>(capture_device_index),
                                  device_name, sizeof(device_name), unique_name, sizeof(unique_name)) != 0){
        Destroy();
        return false;
    }
    vcm_ = webrtc::VideoCaptureFactory::Create(unique_name);
    if(!vcm_){
        return false;
    }
    vcm_->RegisterCaptureDataCallback(this);
    device_info->GetCapability(vcm_->CurrentDeviceName(), 0, capability_);
    capability_.width = static_cast<int32_t>(width);
    capability_.height = static_cast<int32_t>(height);
    capability_.maxFPS = static_cast<int32_t>(target_fps);
    capability_.videoType = webrtc::VideoType::kI420;
    if(vcm_->StartCapture(capability_) != 0){
        Destroy();
        return false;
    }
#else
    // m_capture = new cv::VideoCapture(capture_device_index);
    // if(!m_capture->isOpened()){
    //     RTC_LOG(INFO) << "open camera failed.";
    //     return false;
    // }
    //
    // m_capture->set(CV_CAP_PROP_FPS, target_fps);
    // m_capture->set(CV_CAP_PROP_FRAME_WIDTH, width);
    // m_capture->set(CV_CAP_PROP_FRAME_HEIGHT, height);
    videoWidth = width;
    videoHeight = height;
    pBufYuvI420 = new unsigned char[videoWidth * videoHeight * 3 / 2];
    pthread_create(&(cameraThread), nullptr, RunCameraThread, this);

#endif
    return true;
}

void VcmCapture::Destroy()
{
#if ALVA_WEBRTC_USE_WEBRTCCAPTURE
#else
    if(pBufYuvI420){
        delete[] pBufYuvI420;
    }
#endif
    if(!vcm_){
        return;
    }
    vcm_->StopCapture();
    vcm_->DeRegisterCaptureDataCallback();
    vcm_ = nullptr;
}
#if ALVA_WEBRTC_USE_WEBRTCCAPTURE

#else
void *VcmCapture::RunCameraThread(void *param)
{
    VcmCapture* Vcm = (VcmCapture*)param;
    int width = Vcm->videoWidth;
    int height = Vcm->videoHeight;
    size_t image_size = width * height;
    size_t I420_size = width * height * 3 / 2;
    while(1){
        // cv::Mat frame;
        // *Vcm->m_capture >> frame;
        // if(!frame.empty()){
        //
        // }
        // cv::Mat frameYUV420P;
        // cv::cvtColor(frame, frameYUV420P, CV_BGR2YUV_I420);
        // memcpy(Vcm->pBufYuvI420, frameYUV420P.data, I420_size);
        RTC_LOG(INFO) << "RunCameraThread";
        Vcm->SendFrame(width, height, Vcm->pBufYuvI420, width,
                       Vcm->pBufYuvI420 + image_size, width / 2,
                       Vcm->pBufYuvI420 + image_size + image_size / 4, width / 2, (uint16_t)0);
    }
}
#endif
