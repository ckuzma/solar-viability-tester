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


#ifndef UDPSOCKET_H
#define UDPSOCKET_H

#include "Socket.h"
#include "Endpoint.h"

/**
UDP Socket
*/
class UDPSocket : public Socket, public WNCInterface {
    
public:
    UDPSocket();
    ~UDPSocket();

    int init(void);

    /** sendTo - send data to the remote host.
    \param remote, a pointer to the endpoint (class)
    \param packet, pointer to the buffer to send to the host.
    \param length The length of the buffer to send.
    \return the number of written bytes on success (>=0) or -1 on failure
     */    
    int sendTo(Endpoint &remote, char *packet, int length);

    /** receiveFrom - receive data from the remote host.
    \param remote, a pointer to the endpoint (class)
    \param packet, The buffer in which to store the data received from the host.
    \param length The maximum length of the buffer.
    \return the number of received bytes on success (>=0) or -1 on failure
     */
    int receiveFrom(Endpoint &remote, char *buffer, int length);

    /** Set blocking or non-blocking mode of the socket and a timeout 
    \param  blocking true for blocking mode, false for non-blocking mode.
    \return none.
    */
    void set_blocking (bool blocking, unsigned int timeout=1500);

    /** Close the socket
    \param none
    \return 0 if closed successfully, -1 on failure
    */
    int close(void);

private:
    bool _is_blocking;
    unsigned int _btimeout;
};  
    
#endif

