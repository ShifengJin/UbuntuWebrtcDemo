#ifndef COMMONFUNCTION_H
#define COMMONFUNCTION_H

#include <QObject>
#include <QString>
#include <QJsonDocument>
#include <QJsonObject>
#include <string>
#include "json/json.h"



bool ConvertToJson(const QString &msg, QJsonObject &ret);

QString GetRandomString(int len);

std::string GetPeerConnectionString();

long long JsonValueToLongLong(const Json::Value &jsonData, std::string k);

#endif // COMMONFUNCTION_H
