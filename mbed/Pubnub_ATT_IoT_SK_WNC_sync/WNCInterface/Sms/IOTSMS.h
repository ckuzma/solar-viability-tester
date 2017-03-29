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

    @file          IOTSMS.h
    @version       1.0
    @date          Oct 2016

======================================================================== */

#ifndef WNCSms_H_
#define WNCSms_H_

#include "mbed.h"
#include "rtos.h"

#include "WNCInterface.h"
#include "RtosTimer.h"
#include "Callback.h"

typedef struct SmsMsg {
        string number;
        string date;
        string time;
        string msg;
    } WNCSmsMsg;

/** Component to use the Short Messages Service (SMS) **/
class WNCSms 
{
private:
  unsigned int m_smsinit;
  void m_smscheck(void);
  uint32_t m_timer;

protected:
  WNCSmsMsg m_MsgText;
  RtosTimer *m_SMStimer;
  struct WncController_fk::WncController::WncSmsList m_smsmsgs;     //use the WncSmsList structure to hold messages
  mbed::Callback<void (WNCSmsMsg&)> m_callback;    //pointer to the call-back function the user specified
  
public:
  WNCSms();             /** Create SMSInterface instance */

  /** Initialize interface **/
  int init(uint32_t poll=0, void ((*cb)(WNCSmsMsg& s))=NULL);

  /** Send a SM
     @param string& receiver's contact number
     @param string& the message to send
     @return 0 on success, error code on failure
   */
  int send(const string& number, const string& message);


  /** Receive a SM
     @param string Pointer to the sender's phone number 
     @param string Pointer to store the the incoming message into
     @return 0 on success, error code on failure
   */
  int get(string& number, string& message);
  
  /** Get the users number to use for SMS messaging
     @param none 
     @return character pointer on success, NULL on failure
   */
  char * getSMSNbr( void );
  
  /** Attached a callback function for a Receive SMS
     @param fp(string&) a function that takes a string as an input argument
   */
  void attach(void (*fp)(WNCSmsMsg& s)) {m_callback.attach(fp);}
 
};

#endif /* WNCSms_H_ */
