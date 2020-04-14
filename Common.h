#ifndef COMMONFUNCTION_H
#define COMMONFUNCTION_H

#include <QObject>
#include <QString>
#include <QJsonDocument>
#include <QJsonObject>
#include <string>

#ifndef ALVA_WEBRTC_SWITCH_ON
#define ALVA_WEBRTC_SWITCH_ON     1
#endif

#ifndef ALVA_WEBRTC_SWITCH_OFF
#define ALVA_WEBRTC_SWITCH_OFF    0
#endif

#ifndef DATACHANNEL
#define DATACHANNEL ALVA_WEBRTC_SWITCH_ON
#endif

#ifndef QTSTR_TO_STDSTR
#define QTSTR_TO_STDSTR(qt) (qt.toLatin1().data())
#endif
#ifndef STDSTR_TO_QTSTR
#define STDSTR_TO_QTSTR(std) (QString::fromStdString(std))
#endif

#ifndef WINDOWID
#define WINDOWID unsigned long
#endif


QString GetRandomString(int len);

std::string GetPeerConnectionString();


#endif // COMMONFUNCTION_H
