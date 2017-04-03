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
#include <cstring>

class WNCInterface;

//
// Set up the defaults in the constructor.  If the caller doesn't change anything
// the APN will be set for AT&T, port #40 and timeout 1.5 seconds
//
Socket::Socket() : 
  _sock_type(-1), 
  _timeout(1500) {
}

Socket::~Socket() {
}


//
// ensure we have a WNC Controller attached and initialized by calling to get the 
// network status,  This will provide us with all the network information.  if we
// are not connected, will return -1.
// 
int Socket::init(int timeout) {

    _timeout = timeout;
    M_LOCK;
    int ret = WNCInterface::_pwnc->getWncNetworkingStats(&WNCInterface::myNetStats)? 0:-1;
    M_ULOCK;
    return ret;
}

//
// Connect this socket to a user specified URL or IP address.  It could be 
// either a TCP or UDP socket. The user is also expected to provide a port #.  
// If the connection failed for any reason return 0, otherwise, return 1;
//
int Socket::connect(char *url, int type, int port) {
  int rslt;
  char address[5];
  
  CHK_WNCFE(( WNCInterface::_pwnc->getWncStatus() == FATAL_FLAG ), fail);
 
  // lets determine if they passed in an IP or a URL
  rslt = std::sscanf(url, "%3u.%3u.%3u.%3u",
            (unsigned int*)&address[0], (unsigned int*)&address[1],
            (unsigned int*)&address[2], (unsigned int*)&address[3]);
  M_LOCK;  
  if (rslt == 4) 
    rslt = WNCInterface::_pwnc->setIpAddr(0,url);
  else 
    rslt = WNCInterface::_pwnc->resolveUrl(0,url);

  if( rslt ) {
    _sock_type = type;             //resolved the URL indicate socket 0 is open
    rslt = WNCInterface::_pwnc->openSocket(0, port, (_sock_type==SOCK_STREAM)? 1:0, _timeout);
    } 
  M_ULOCK;
  return rslt;
} 


//
// disconnect the currently open socket.
// -1 if there was an error
//  0 if we disconnected
//
int Socket::disconnect() {
  if( _sock_type<0 )
    return 0;  //nothing is connected currently

  CHK_WNCFE(( WNCInterface::_pwnc->getWncStatus() == FATAL_FLAG ), fail);
  M_LOCK;
  int ret = !WNCInterface::_pwnc->closeSocket(0);
  M_ULOCK;
  return ret;
}

void Socket::set_blocking(bool blocking, unsigned int timeout) {
    blocking = blocking;
    timeout= timeout;

    CHK_WNCFE(( WNCInterface::_pwnc->getWncStatus() == FATAL_FLAG ), void);
    M_LOCK;
    WNCInterface::_pwnc->setReadRetryWait(0, 0);
    WNCInterface::_pwnc->setReadRetries(0, 0);
    M_ULOCK;
}


