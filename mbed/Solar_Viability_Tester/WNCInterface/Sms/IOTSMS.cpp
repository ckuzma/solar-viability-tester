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

    @file          IOTSMS.cpp
    @version       1.0
    @date          Oct 2016

======================================================================== */

#ifndef __MODULE__
#define __MODULE__ "WNCSms.cpp"
#endif

#include "IOTSMS.h"
#include "mbed.h"
#include "rtos.h"

#include <cstdio>
#include <cstring>

//
// this is a user specified function that checks for SMS messages if enabled.  It will
// check to see if a message is available, and if there is, it will call the users
// supplied call-back function with the message.
//

WNCSms::WNCSms(void ) : m_smsinit(false)  
{
    m_SMStimer = new RtosTimer(this, &WNCSms::m_smscheck, osTimerPeriodic);
    m_SMStimer->stop();
}

void WNCSms::m_smscheck( void ){
  
  //we don't know what the frequency was the user selected to check for
  //SMS messages, but we don't want to run into the problem where we are
  //not completing our tasks wihin the timeperiod.  So stop the timer when
  //we come in and start it when we leave.
  //
  CHK_WNCFE(( WNCInterface::_pwnc->getWncStatus() == FATAL_FLAG ), void);
    
  m_SMStimer->stop();  //stop the timer while we check for SMS messages
  M_LOCK;       
  if( WNCInterface::_pwnc->readUnreadSMSText(&m_smsmsgs, true) ) {
    for( int i=0; i< m_smsmsgs.msgCount; i++ ) {
        m_MsgText.number = m_smsmsgs.e[i].number;
        m_MsgText.date = m_smsmsgs.e[i].date;
        m_MsgText.time = m_smsmsgs.e[i].time;
        m_MsgText.msg = m_smsmsgs.e[i].msg;
        m_callback.call(m_MsgText);
        }
    }
  M_ULOCK;
  m_SMStimer->start(m_timer);  //start it back up
}

//
// initialize the IoT SMS class.  Allow a user to set a polling period to check for
// SMS messages, and if one is recevied, call a uer defined function.  A caller can
// not ask for a polling period with no call-back function, and they can not provide
// a callback function with no polling period.
//
// Prior to initializing WNCSms, you must call and create a WNCInterface class object 
// because the WNCSms object utilizes services of WNCInterface.
//
// Input:     poll - polling frequency in sec.
//      m_callback - pointer to a user defined function
// Output:
//      0 if no error 
//   != 0 if any error
//
int WNCSms::init(uint32_t p, void (*cb)(WNCSmsMsg& s)) {

  CHK_WNCFE(( WNCInterface::_pwnc->getWncStatus() == FATAL_FLAG ), fail);
  
  if( WNCInterface::_pwnc == NULL )  //must have already initialized WNCInterface
    return -1;

  if(( p == 0 && cb != NULL ) || (p != 0 && cb == NULL)) 
    return -1;
    
  if( p != 0 && cb != NULL ) {
    m_callback.attach(cb);
    m_timer = p*1000;
    m_SMStimer->start(m_timer);
    }
    
  M_LOCK;
  WNCInterface::_pwnc->deleteSMSTextFromMem('*');  //delete any message currently in storage
  M_ULOCK;
  m_smsinit = true;

  return 0;
}

//
// send a message to a user specified number.
//
int WNCSms::send(const string& number, const string& message) {

  if( !m_smsinit )
    return -1;
  M_LOCK;
  int ret=WNCInterface::_pwnc->sendSMSText(number.c_str(), message.c_str());
  M_ULOCK;
  return ret;
  }


//
// get a message and put it into the callers string buffer.  When returned, this method will
// let the caller know that there are more SMS messages to get or 0 when all messages have
// been read.
//
// input:  pointer to a string for the senders number
//         pointer to a string for the senders message
// output: 0 if no additional messages available
//        !0 if additional message available
//

int WNCSms::get(string& number, string& message) {
  if( !m_smsinit )
    return -1;

  CHK_WNCFE(( WNCInterface::_pwnc->getWncStatus() == FATAL_FLAG ), fail);
  M_LOCK;
  int ret = WNCInterface::_pwnc->readUnreadSMSText(&m_smsmsgs, true);
  M_ULOCK;
  return ret;
  }
  

char * WNCSms::getSMSNbr( void ) {
    char * ret=NULL;
    string iccid_str;
    static string msisdn_str;
    
    CHK_WNCFE(( WNCInterface::_pwnc->getWncStatus() == FATAL_FLAG ), null);
    M_LOCK;
    if( !WNCInterface::_pwnc->getICCID(&iccid_str) ) {
       M_ULOCK;
       return NULL;
       }
      
    CHK_WNCFE(( WNCInterface::_pwnc->getWncStatus() == FATAL_FLAG ), null);

    if( WNCInterface::_pwnc->convertICCIDtoMSISDN(iccid_str, &msisdn_str) )
        ret = (char*)msisdn_str.c_str();    
    M_ULOCK;
    return ret;
}
    
