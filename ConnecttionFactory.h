#ifndef CONNECTTIONFACTORY_H
#define CONNECTTIONFACTORY_H

#include <webrtc/api/peer_connection_interface.h>

class ConnecttionFactory
{
public:
    static bool Init();
    static void Unit();
    static rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> Get();

private:
    static rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> _peer_connection_factory;
};

#endif // CONNECTTIONFACTORY_H
