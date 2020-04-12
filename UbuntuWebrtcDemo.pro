#-------------------------------------------------
#
# Project created by QtCreator 2019-10-24T10:27:49
#
#-------------------------------------------------

QT       += core gui network websockets widgets

TARGET = WebRTCDemo_20191024
TEMPLATE = app

INCLUDEPATH += .

CONFIG += c++11

DEFINES += QT_DEPRECATED_WARNINGS QT_NO_KEYWORDS
DEFINES += HAVE_WEBRTC_VOICE WEBRTC_INCLUDE_INTERNAL_AUDIO_DEVICE WEBRTC_USE_H264 WEBRTC_INITIALIZE_FFMPEG FFMPEG_H264_DECODER WEBRTC_POSIX

unix: !mac{

QMAKE_CFLAGS = -fno-rtti -pipe
QMAKE_CXXFLAGS = -fno-rtti -pipe

INCLUDEPATH+=$$PWD/webrtc/include
INCLUDEPATH+=$$PWD/webrtc/include/webrtc

INCLUDEPATH+=$$PWD/libyuv/include

LIBS+=-L$$PWD/webrtc -lwebrtc -lpthread -ldl -lm -lX11
LIBS+=-L$$PWD/libyuv -lyuv

LIBS+=-L$$PWD/jsoncpp -ljsoncpp
INCLUDEPATH+=$$PWD/jsoncpp/include

# INCLUDEPATH += /usr/local/include/opencv
# INCLUDEPATH += /usr/local/include/opencv2

# LIBS += /usr/local/lib/libopencv_highgui.so \
#         /usr/local/lib/libopencv_core.so \
#         /usr/local/lib/libopencv_imgproc.so \
#         /usr/local/lib/libopencv_imgcodecs.so \
#         /usr/local/lib/libopencv_shape.so \
#         /usr/local/lib/libopencv_videoio.so \
#         /usr/local/lib/libopencv_calib3d.so \
#         /usr/local/lib/libopencv_features2d.so \
#         /usr/local/lib/libopencv_flann.so \
#         /usr/local/lib/libopencv_ml.so \
#         /usr/local/lib/libopencv_objdetect.so \
#         /usr/local/lib/libopencv_photo.so \
#         /usr/local/lib/libopencv_stitching.so \
#         /usr/local/lib/libopencv_superres.so \
#         /usr/local/lib/libopencv_video.so \
#         /usr/local/lib/libopencv_videostab.so



}

SOURCES += \
    JsonTools.cpp \
        main.cpp \
        MainWindow.cpp \
    QtWebrtcStream.cpp \
    QtWebRTCVideoFrame.cpp \
    QVideoRender.cpp \
    WebRTCInterface.cpp \
    CommonFunction.cpp \
    ConnecttionFactory.cpp \
    QtConferenceManager.cpp \
    JanusWebSocket.cpp \
    JanusVideoRoomManager.cpp \
    JanusPeerConnection.cpp \
    QtWebrtcRemoteStream.cpp \
    CapturerTrackSource.cpp \
    TestVideoCapture.cpp \
    VcmCapture.cpp

HEADERS += \
    JsonTools.h \
        MainWindow.h \
    QtWebrtcStream.h \
    QtWebRTCVideoFrame.h \
    CommonDefine.h \
    QVideoRender.h \
    WebRTCInterface.h \
    CommonFunction.h \
    ConnecttionFactory.h \
    QtConferenceManager.h \
    JanusWebSocket.h \
    JanusVideoRoomManager.h \
    JanusPeerConnection.h \
    QtWebrtcRemoteStream.h \
    CapturerTrackSource.h \
    TestVideoCapture.h \
    VcmCapture.h


FORMS += \
        MainWindow.ui
