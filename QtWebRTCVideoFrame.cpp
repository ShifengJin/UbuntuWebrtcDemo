#include "QtWebRTCVideoFrame.h"

#include <QDebug>

#include <webrtc/api/video/i420_buffer.h>
#include <webrtc/rtc_base/arraysize.h>
#include <libyuv/convert_argb.h>
#include "QVideoRender.h"

QtWebRTCVideoFrame::QtWebRTCVideoFrame(unsigned long iWindowID)
{
    this->mWindowID = iWindowID;
}

QtWebRTCVideoFrame::~QtWebRTCVideoFrame()
{
    this->mWindowID = -1;
}

void QtWebRTCVideoFrame::OnFrame(const webrtc::VideoFrame &iFrame)
{
    rtc::scoped_refptr<webrtc::I420BufferInterface> buffer(iFrame.video_frame_buffer()->ToI420());
    if(iFrame.rotation() != webrtc::kVideoRotation_0){
        buffer = webrtc::I420Buffer::Rotate(*buffer, iFrame.rotation());
    }

    QVideoRender *paintWidget = (QVideoRender*)QWidget::find(mWindowID);;

    if(paintWidget == nullptr)
    {
       qDebug() << "paintWidget nullptr,ERROR ,return";
       return;
    }

    int yuvBufferSize = buffer->width() * buffer->height() * 3 / 2;
    unsigned char * yuvBuffer = (unsigned char*)malloc(yuvBufferSize);

    RTC_CHECK(yuvBuffer!=nullptr);

    qDebug() << "width : " << buffer->width() << "    height : " <<   buffer->height();
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
}

unsigned long QtWebRTCVideoFrame::GetWindowID() const
{
    return this->mWindowID;
}

void QtWebRTCVideoFrame::SetWindowID(unsigned long iWindowID)
{
    this->mWindowID = iWindowID;
}
