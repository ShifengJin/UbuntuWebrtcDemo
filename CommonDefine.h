#ifndef COMMONDEFINE_H
#define COMMONDEFINE_H

#define ALVA_WEBRTC_SWITCH_ON     1
#define ALVA_WEBRTC_SWITCH_OFF    0

#define DATACHANNEL ALVA_WEBRTC_SWITCH_ON

#define QTSTR_TO_STDSTR(qt) (qt.toLatin1().data())
#define STDSTR_TO_QTSTR(std) (QString::fromStdString(std))

#define WINDOWID unsigned long

#endif // COMMONDEFINE_H
