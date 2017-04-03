/* =====================================================================
   Copyright © 2016, Avnet (R)

   Contributors:
     * James M Flynn, www.em.avnet.com 
 
   Licensed under the Apache License, Version 2.0 (the "License"); 
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, 
   software distributed under the License is distributed on an 
   "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, 
   either express or implied. See the License for the specific 
   language governing permissions and limitations under the License.

    @file          WNCInterface.cpp
    @version       1.0
    @date          Sept 2016

======================================================================== */

#include "../WNCInterface.h"

#include "UDPSocket.h"
#include <cstring>

UDPSocket::UDPSocket() :
    _is_blocking(0),
    _btimeout(0){
}

UDPSocket::~UDPSocket() {
}

int UDPSocket::init(void) {
    _is_blocking = false;   // start out not blocking, user will set it if desired
    return ( WNCInterface::_pwnc->getWncStatus() == WNC_GOOD )? 0:-1;
}


int UDPSocket::close(void) {
    Socket::disconnect();
    return ( WNCInterface::_pwnc->getWncStatus() == WNC_GOOD )? 0:-1;
}

// -1 if unsuccessful, else number of bytes written
int UDPSocket::sendTo(Endpoint &remote, char *packet, int length) {
    int ret = -1;
    
    CHK_WNCFE(( WNCInterface::_pwnc->getWncStatus() == FATAL_FLAG ), fail);
    if( remote._epAddr.port ) {  //make sure the Endpoint has an port assoicated with it
        if( Socket::connect(remote._epAddr.IP,SOCK_DGRAM,remote._epAddr.port) ) {
            if( WNCInterface::_pwnc->write(0,packet,length) )
                ret = length;
            close();
        }
    }
    return ret;
}

//
// blocking is used to make the WNC keep checking for incoming data for a
// period of time.

void UDPSocket::set_blocking (bool blocking, unsigned int timeout) {
    _is_blocking = blocking;   // true or false
    _btimeout = timeout;       // user specifies in msec
 
    CHK_WNCFE(( WNCInterface::_pwnc->getWncStatus() == FATAL_FLAG ), void);
    WNCInterface::_pwnc->setReadRetryWait(0, 0);
    WNCInterface::_pwnc->setReadRetries(0, 0);
}

// -1 if unsuccessful, else number of bytes received
int UDPSocket::receiveFrom(Endpoint &remote, char *buffer, int length) {
    const uint8_t *ptr;
    Timer t;
    int done, ret = -1;
    
    //make sure the Endpoint has an port assoicated with it
    if( !remote._epAddr.port )
      return -1;

    CHK_WNCFE(( WNCInterface::_pwnc->getWncStatus() == FATAL_FLAG ), fail);
    ret = Socket::connect(remote._epAddr.IP,SOCK_DGRAM,remote._epAddr.port);        
    
    t.start();
    do {
        ret = WNCInterface::_pwnc->read(0, &ptr);
        done = ret | (t.read_ms() > _btimeout);
        }
    while( _is_blocking && !done );
    t.stop();
    
    if( ret > length )
        ret = length;
    memcpy( buffer, ptr, ret );
    
    return ret;
}

   
