
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <QDebug>
#include <webrtc/modules/video_capture/video_capture.h>
#include <webrtc/modules/video_capture/video_capture_factory.h>
#include <webrtc/rtc_base/arraysize.h>

#include <unistd.h>
#include "CommonFunction.h"
#include "AlvaJsonTools.h"
#include "json/json.h"

bool ConvertToJson(const QString &msg, QJsonObject &ret)
{
    QJsonParseError parseError;
    QJsonDocument document = QJsonDocument::fromJson(msg.toLatin1(), &parseError);
    if (parseError.error != QJsonParseError::NoError)
    {
        return false;
    }

    if (!document.isObject())
    {
        return false;
    }

    ret = document.object();
    return true;
}

QString GetRandomString(int len)
{
    static const QString charSet = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    static int strLen = charSet.length();

    QString randomString;
    for (int i = 0; i < len; i++)
    {
        randomString += charSet.at(qrand() % strLen);
    }
    return randomString;
}

std::string GetEnvVarOrDefault(const char* env_var_name,
                               const char* default_value)
{
    std::string value;
    const char* env_var = getenv(env_var_name);
    if(env_var)
        value = env_var;

    if(value.empty())
        value = default_value;

    return value;
}
std::string GetPeerConnectionString()
{
    return GetEnvVarOrDefault("WEBRTC_CONNECT","stun:192.168.1.198");
}

std::string GetDefaultServerName()
{
    return GetEnvVarOrDefault("WEBRTC_SERVER","192.168.1.198");
}

std::string GetPeerName()
{
    char Computer_name[256];
    std::string ret(GetEnvVarOrDefault("USERNAME","user"));
    ret += '@';
    if(gethostname(Computer_name,arraysize(Computer_name)) == 0)
    {
        ret += Computer_name;
    }
    else
    {
        ret += "host";
    }
    return ret;
}


long long JsonValueToLongLong(const Json::Value &jsonData, std::string k)
{
    Json::Value idStr;
    GetValueFromJsonObject(jsonData, k, &idStr);
    std::string id_stdStr = JsonValueToString(idStr);
    long long returnValue = atoll(id_stdStr.c_str());
    return returnValue;
}
