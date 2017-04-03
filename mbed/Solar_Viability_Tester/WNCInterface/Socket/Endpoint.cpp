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
#include "Socket.h"
#include "Endpoint.h"

Endpoint::Endpoint()  {
    reset_address();
}

Endpoint::~Endpoint() {}

void Endpoint::reset_address(void) {
    std::memset(&_epAddr, 0, sizeof(struct EndPointAddr));
}

//
// It is possible to call set_address with either a URL or
// an IP address.  So try each in-turn and set the end point
// address.
//

int Endpoint::set_address(const char* host, const int port) {
     // IP Address
    char address[5];
    int rslt;

   CHK_WNCFE(( WNCInterface::_pwnc->getWncStatus() == FATAL_FLAG ), fail);

   reset_address();
    _epAddr.port = port;  //go ahead and save the port 
    
    // Dot-decimal notation?
    rslt = std::sscanf(host, "%3u.%3u.%3u.%3u",
            (unsigned int*)&address[0], (unsigned int*)&address[1],
            (unsigned int*)&address[2], (unsigned int*)&address[3]);

    M_LOCK;
    if (rslt != 4)    // No, need to resolve address with DNS
        WNCInterface::_pwnc->resolveUrl(0,host);
    else
        WNCInterface::_pwnc->setIpAddr(0,host);
    
    rslt = WNCInterface::_pwnc->getIpAddr(0,_epAddr.IP);
    M_ULOCK;
    return rslt;
}

char* Endpoint::get_address() {
    return _epAddr.IP;
}

int   Endpoint::get_port() {
    return _epAddr.port;
}

