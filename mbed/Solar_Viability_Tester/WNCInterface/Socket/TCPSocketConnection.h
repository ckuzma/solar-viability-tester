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

#ifndef TCPSOCKET_H
#define TCPSOCKET_H

#include "Socket/Socket.h"
#include "Socket/Endpoint.h"

/**
TCP socket connection
*/
class TCPSocketConnection : public Socket, public Endpoint {
    
public:
    TCPSocketConnection();

    /** Connects this TCP socket to the server
    \param host The host to connect to. It can either be an IP Address or a hostname that will be resolved with DNS.
    \param port The host's port to connect to.
    \return 0 on success, -1 on failure.
    */    
    int connect(const char* host, const int port);

    /** Check if the socket is connected
    \return true if connected, false otherwise.
    */    
    bool is_connected(void);

    /** Send data to the remote host.
    \param data The buffer to send to the host.
    \param length The length of the buffer to send.
    \return the number of written bytes on success (>=0) or -1 on failure
     */    
    int send(char* data, int length);
    
    /** Send all the data to the remote host.
    \param data The buffer to send to the host.
    \param length The length of the buffer to send.
    \return the number of written bytes on success (>=0) or -1 on failure
    */
    int send_all(char* data, int length);
    
    /** Receive data from the remote host.
    \param data The buffer in which to store the data received from the host.
    \param length The maximum length of the buffer.
    \return the number of received bytes on success (>=0) or -1 on failure
     */
    int receive(char* data, int length);
    
    /** Receive all the data from the remote host.
    \param data The buffer in which to store the data received from the host.
    \param length The maximum length of the buffer.
    \return the number of received bytes on success (>=0) or -1 on failure
    */
    int receive_all(char* data, int length);

    /** Set blocking or non-blocking mode of the socket and a timeout 
    \param  blocking true for blocking mode, false for non-blocking mode.
    \return none
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
