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

#include "Socket.h"

#ifndef ENDPOINT_H
#define ENDPOINT_H

struct EndPointAddr {
  char IP[16];
  unsigned int port;
  };

class UDPSocket;

class Endpoint {
    friend class UDPSocket;

public:
    Endpoint(void);
    ~Endpoint(void);
    
    /** Reset the address of the endpoint by clearning the internal endpoint IP address
    \param none
    \return none.
     */
    void reset_address(void);

    /** Set the address of the endpoint
    \param host The endpoint address (it can either be an IP Address or a hostname that will be resolved with DNS).
    \param port The endpoint port
    \return 0 on success, -1 on failure (when an hostname cannot be resolved by DNS).
     */
    int  set_address(const char* host, const int port);

    /** Get the IP address of the endpoint
    \return The IP address of the endpoint.
     */
    char* get_address(void); 

    /** Get the port of the endpoint
    \return The port of the endpoint
     */
    int get_port(void);

private:
    EndPointAddr _epAddr;
};
  
#endif
