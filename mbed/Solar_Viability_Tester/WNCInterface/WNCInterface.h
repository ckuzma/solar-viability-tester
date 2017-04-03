/* =====================================================================
   Copyright © 2016, Avnet (R)

   Contributors:
     * James Flynn, www.em.avnet.com 
 
   Licensed under the Apache License, Version 2.0 (the "License"); 
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, 
   software distributed under the License is distributed on an 
   "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, 
   either express or implied. See the License for the specific 
   language governing permissions and limitations under the License.

    @file          WNCInterface.h
    @version       1.0
    @date          Sept 2016

======================================================================== */


#include <stddef.h>
#include "WncControllerK64F/WncControllerK64F.h"
#include "WncControllerK64F/WncController/WncController.h"
#include <Mutex.h>

#ifndef _WNCINTERFACE_H_
#define _WNCINTERFACE_H_

#define WNC_DEBUG   0   //1=enable the WNC startup debug output
                                //0=disable the WNC startup debug output
#define STOP_ON_FE  1   //1=hang forever if a fatal error occurs
                //0=simply return failed response for all socket calls
#define DISPLAY_FE  1   //1 to display the fatal error when it occurs
                //0 to NOT display the fatal error
#define RESETON_FE  0   //1 to cause the MCU to reset on fatal error
                //0 to NOT reset the MCU
#define APN_DEFAULT     "m2m.com.attz"
                        
//
// WNC Error Handling macros & data
//
#define FATAL_FLAG  WncController_fk::WncController::WNC_NO_RESPONSE
#define WNC_GOOD    WncController_fk::WncController::WNC_ON

#define RETfail return -1
#define RETvoid return
#define RETnull return NULL
#define RETresume   

#define DORET(x) RET##x

#define TOSTR(x) #x
#define INTSTR(x) TOSTR(x)
#define FATAL_STR __FILE__ ":" INTSTR(__LINE__)


#if RESETON_FE == 1
#define MCURESET     ((*((volatile unsigned long *)0xE000ED0CU))=(unsigned long)((0x5fa<<16) | 0x04L))
#define RSTMSG       "RESET MCU! "
#else
#define MCURESET
#define RSTMSG       ""
#endif

#if DISPLAY_FE == 1
#define PFE     {extern MODSERIAL *_dbgout;if(_dbgout)_dbgout->printf(RSTMSG "\r\n>>WNC FAILED @ %s\r\n", FATAL_STR);}
#else
#define PFE
#endif

#if STOP_ON_FE == 1
#define FATAL_WNC_ERROR(v)  {extern char *_fatal_err_loc;_fatal_err_loc=FATAL_STR;PFE;MCURESET;while(1);}
#else
#define FATAL_WNC_ERROR(v)  {extern char *_fatal_err_loc;_fatal_err_loc=FATAL_STR;PFE;DORET(v);}
#endif

#define M_LOCK  {extern Mutex _WNCLock; _WNCLock.lock();}
#define M_ULOCK {extern Mutex _WNCLock; _WNCLock.unlock();}
#define CHK_WNCFE(x,y)    if( x ){M_ULOCK; FATAL_WNC_ERROR(y);}

// Because the WncController has intermixed socket & interface functionallity
// will need to make the Socket class a friend of the Interface class. This 
// will allow the Socket class to get to the WNC functions needed for the
// socket. Forward reference the class

class Socket;
class Endpoint;
class UDPSocket;
class TCPSocketConnection;
class WNCSms;

class WNCInterface
{
  class WncControllerK64F;  //forward reference the Controller Class
  friend class TCPSocketConnection;
  friend class UDPSocket;
  friend class Endpoint;
  friend class Socket;
  friend class WNCSms;

public:
  /** Create WNC Data Module Interface Instance for the device (M14A2A) */
  WNCInterface();

  /** Initialize the interface (no connection at this point).
  * \return 0 on success, a negative number on failure
  */
  static int init(const char* apn=NULL, MODSERIAL * debug=NULL); 

  /** Open an LTE internet data connection
      @return 0 on success, error code on failure
  */
  int connect(void);

  /** Disconnect
  * Bring the interface down
  * \return 0 on success, a negative number on failure
  */
  static int disconnect();
  
  /** Because the WNCInterface is cellular based there is no MAC Ethernet address to return, so this function
   *  returns a bogus MAC address created from the ICCD on the SIM that is being used.
   * \return a pointer to a pesudo-MAC string containing "NN:NN:NN:NN:NN:NN"
   */
  static char* getMACAddress();
  
  /** Get the IP address of your Ethernet interface
   * \return a pointer to a string containing the IP address
   */
  static char* getIPAddress();

  /** Get the Gateway address of your Ethernet interface
   * \return a pointer to a string containing the Gateway address
   */
  static char* getGateway();

  /** Get the Network mask of your Ethernet interface
   * \return a pointer to a string containing the Network mask
   */
  static char* getNetworkMask();
  
  /** Manipulate the debug output of the WncController, for debug purposes.
   * \return nothing.
   */
  void doDebug(int val);  //doing this so I can get to the wnc controller pointer
  
  
private:
  static WncController_fk::WncIpStats myNetStats;     //maintaint the network statistics
  static WncControllerK64F_fk::WncControllerK64F *_pwnc;  //pointer to the WncController instance
  static string mac;

};

#endif /* _WNCINTERFACE_ */

