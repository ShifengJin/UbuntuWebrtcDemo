#ifndef ALVAWEBRTCCALLBACKDEFINE_H
#define ALVAWEBRTCCALLBACKDEFINE_H

#include <string>
#include <QString>

typedef std::function<void(long long, std::string, std::string)> SENDSDP_CALLBACK;
typedef std::function<void(long long, std::string, int, std::string)> SENDICECANDIDATE_CALLBACK;
typedef std::function<void(long long)> ICEGATHERINGCOMPLETE_CALLBACK;

typedef std::function<void(long long, bool, bool, bool)> CONNECTTOPEER_CALLBACK;
typedef std::function<void(long long, std::string, std::string, bool isTextRoom)> REMOTESDP_CALLBACK;
typedef std::function<void(long long)> REMOTESTREAMREMOVE_CALLBACK;

typedef std::function<void(std::string)> RECVMESSAGE_CALLBACK;
typedef std::function<void(std::string&)> SENDLOCALINFOWHENOPENDATACHANNEL_CALLBACK;

#endif // ALVAWEBRTCCALLBACKDEFINE_H
