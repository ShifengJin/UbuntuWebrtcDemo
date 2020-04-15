#ifndef VCMCAPTURE_H
#define VCMCAPTURE_H

#include <memory>
#include <vector>

#include <webrtc/api/scoped_refptr.h>
#include <webrtc/modules/video_capture/video_capture.h>
#include "VideoCapture.h"

#define ALVA_WEBRTC_USE_WEBRTCCAPTURE 1

#if ALVA_WEBRTC_USE_WEBRTCCAPTURE

#else
#include <pthread.h>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/videoio.hpp>
#endif


#if ALVA_WEBRTC_USE_WEBRTCCAPTURE
class VcmCapture : public VideoCapture,
                   public rtc::VideoSinkInterface<webrtc::VideoFrame>
#else
class VcmCapture : public VideoCapture
#endif
{
public:

    virtual ~VcmCapture();
#if ALVA_WEBRTC_USE_WEBRTCCAPTURE
    void OnFrame(const webrtc::VideoFrame& frame) override;
#else
    void OnFrame(const webrtc::VideoFrame& frame);

    void SendFrame(int width,
                   int height,
                   const uint8_t* data_y,
                   int stride_y,
                   const uint8_t* data_u,
                   int stride_u,
                   const uint8_t* data_v,
                   int stride_v,
                   uint16_t frame_id);

#endif
    VcmCapture(size_t width, size_t height, size_t target_fps, size_t capture_device_index);

    void SetWindowID(WINDOWID inWindowID){mWindowID = inWindowID;}

private:
    VcmCapture();

    bool Init(size_t width,
              size_t height,
              size_t target_fps,
              size_t capture_device_index);

    void Destroy();

    rtc::scoped_refptr<webrtc::VideoCaptureModule> vcm_;
    webrtc::VideoCaptureCapability capability_;

#if ALVA_WEBRTC_USE_WEBRTCCAPTURE

#else

    static void * RunCameraThread(void* param);
    int videoWidth;
    int videoHeight;
    unsigned char* pBufYuvI420;
    cv::VideoCapture   *m_capture;
    pthread_t           cameraThread;
#endif


    WINDOWID                                        mWindowID;
};

#endif // VCMCAPTURE_H
