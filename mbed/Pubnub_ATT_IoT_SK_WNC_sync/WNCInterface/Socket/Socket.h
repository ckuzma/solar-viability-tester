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


#include <stddef.h>
#include "WNCInterface.h"

#ifndef SOCKET_H_
#define SOCKET_H_

#define SOCK_STREAM     1  //A TCP Socket type
#define SOCK_DGRAM      2  //a UDP Socket type

/** Socket file descriptor and select wrapper */
class Socket {
  
public:
    Socket();
    ~Socket();

    int init(int timeout=1500);

    int connect(char *url, int type, int port);
    int disconnect();
    void set_blocking(bool blocking, unsigned int timeout); //not used
    
private:
    int _sock_type;                     //contains the type of socket this is
    unsigned int _timeout;              //default timeout for all socket transactions
};


#endif /* SOCKET_H_ */
