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
#include "TCPSocketConnection.h"
#include <cstring>

#define READ_EVERYMS   500     //number of milliseconds between WNC socket reads

TCPSocketConnection::TCPSocketConnection() :
        _is_blocking(0),
        _btimeout(0){
}

//
// blocking is used to make the WNC keep checking for incoming data for a
// period of time.
//
void TCPSocketConnection::set_blocking (bool blocking, unsigned int timeout) {
    _is_blocking = blocking;   // true if we want to wait for request
    _btimeout = timeout;       // user specs msec

    CHK_WNCFE(( WNCInterface::_pwnc->getWncStatus() == FATAL_FLAG ), void);
    M_LOCK;
    WNCInterface::_pwnc->setReadRetryWait(0, 0);
    WNCInterface::_pwnc->setReadRetries(0, 0);
    M_ULOCK;
}


int TCPSocketConnection::connect(const char* host, const int port) {
    Socket::connect((char*)host, SOCK_STREAM, port);
    _is_blocking = false;   // start out not blocking, user will set it if desired
    return ( WNCInterface::_pwnc->getWncStatus() == WncController_fk::WncController::WNC_ON )? 0:-1;
}

bool TCPSocketConnection::is_connected(void) {
    return ( WNCInterface::_pwnc->getWncStatus() == WncController_fk::WncController::WNC_ON )? 1:0;
}

int TCPSocketConnection::send(char* data, int length) {
    int ret = -1;
    
    WncController_fk::WncController::WncState_e s = WNCInterface::_pwnc->getWncStatus();

    CHK_WNCFE(( s == FATAL_FLAG ), fail);
 
    if( s == WncController_fk::WncController::WNC_ON ) {
      M_LOCK;
      if( WNCInterface::_pwnc->write(0, data, length) )
        ret = length;
      M_ULOCK;
      }
    return ret;
}

int TCPSocketConnection::receive(char *readBuf, int length) {
    Timer t;
    size_t done, cnt;
    int ret=-1;
    WncController_fk::WncController::WncState_e s = WNCInterface::_pwnc->getWncStatus();

    CHK_WNCFE(( s  == FATAL_FLAG ), fail);
    if( s != WncController_fk::WncController::WNC_ON )
        return ret;

    M_LOCK;
    t.start();
    do {
        if( !(t.read_ms() % READ_EVERYMS) )
          cnt = WNCInterface::_pwnc->read(0, (uint8_t *)readBuf, (uint32_t) length);
        if( _is_blocking )
            done = cnt;
        else
            done = cnt | (t.read_ms() > _btimeout);
        }
    while( !done );
    t.stop();
    M_ULOCK;
    
    if( WNCInterface::_pwnc->getWncStatus() == WNC_GOOD ) {
        //readBuf[cnt] = '\0';
        ret = (int)cnt;
        }
    else
        ret = -1;
    
    return ret;
}

int TCPSocketConnection::send_all(char* data, int length) {
  return send(data,length);
}

int TCPSocketConnection::receive_all(char* data, int length) {
  return receive(data,length);
}

int TCPSocketConnection::close(void) {
  Socket::disconnect();
  M_LOCK;
  int ret = ( WNCInterface::_pwnc->getWncStatus() == WncController_fk::WncController::WNC_ON )? 0:-1;
  M_ULOCK;
  return ret;
}

