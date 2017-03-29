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
    @author        James Flynn

======================================================================== */


#ifndef __MODULE__
#define __MODULE__ "WNCInterface.cpp"
#endif

#include "WNCInterface.h"

/////////////////////////////////////////////////////
// NXP GPIO Pins that are used to initialize the WNC Shield
/////////////////////////////////////////////////////
DigitalOut  mdm_uart2_rx_boot_mode_sel(PTC17);  // on powerup, 0 = boot mode, 1 = normal boot
DigitalOut  mdm_power_on(PTB9);                 // 0 = turn modem on, 1 = turn modem off (should be held high for >5 seconds to cycle modem)
DigitalOut  mdm_wakeup_in(PTC2);                // 0 = let modem sleep, 1 = keep modem awake -- Note: pulled high on shield
DigitalOut  mdm_reset(PTC12);                   // active high
DigitalOut  shield_3v3_1v8_sig_trans_ena(PTC4); // 0 = disabled (all signals high impedence, 1 = translation active
DigitalOut  mdm_uart1_cts(PTD0);

char * _fatal_err_loc;                          //GLOBAL::holds any error location info
MODSERIAL * _dbgout;
Mutex _WNCLock;

using namespace WncControllerK64F_fk;       // namespace for the controller class use

// Define pin associations for the controller class to use be careful to 
//  keep the order of the pins in the initialization list.
WncGpioPinListK64F wncPinList = { 
    &mdm_uart2_rx_boot_mode_sel,
    &mdm_power_on,
    &mdm_wakeup_in,
    &mdm_reset,
    &shield_3v3_1v8_sig_trans_ena,
    &mdm_uart1_cts
};

static MODSERIAL mdmUart(PTD3,PTD2,256,4096);       //UART for WNC Module

WncControllerK64F *WNCInterface::_pwnc;
WncIpStats WNCInterface::myNetStats;
string WNCInterface::mac;
    
WNCInterface::WNCInterface() {
    _dbgout = NULL;
}

void WNCInterface::doDebug( int v ) {
    //basic debug = 0x01
    //more debug  = 0x02
    //all debug   = 0x03
    M_LOCK;
    _pwnc->enableDebug( (v&1), (v&2) );
    M_ULOCK;
}
    
//
// Power-up the WNC module.  The caller can optionally configure.
// Inputs:
//    apn - Caller can specify an APN. If none is provided will use "m2m.com.attz"
//   debug- specify the amount of debug the WNC controller should output:
//          1 - Basic Debug output
//          2 - Verbose Debug output
//          3 - Full Debug output
// Returns: 0 if unable to initialize the WNC module
//         -1 if successfully initialized
//
int WNCInterface::init(const char* apn, MODSERIAL * debug) {
    int ret = 0;
    
    M_LOCK;
    if( debug ) {
        _dbgout = debug;
        _pwnc = new WncControllerK64F_fk::WncControllerK64F::WncControllerK64F(&wncPinList, &mdmUart, debug);
#if WNC_DEBUG == 1
    _pwnc->enableDebug(1,1);
#endif
        }
    else
        _pwnc = new WncControllerK64F_fk::WncControllerK64F::WncControllerK64F(&wncPinList, &mdmUart, NULL);

    if( apn==NULL )
        apn = APN_DEFAULT;

    ret =  ( _pwnc->powerWncOn(apn,40) )? 2:0;
    ret |= ( _pwnc->setApnName(apn) )? 1:0;
    ret |= ( _pwnc->getWncNetworkingStats(&myNetStats) )? 4:0;
    M_ULOCK;
    
    return ret;
}

//
// check to see if we are connected to the internet or not. The 
// connection is supposed to happen during init.  If we are
// connected to the internet return 0 otherwise return -1
//
int WNCInterface::connect(void) {
    return ( _pwnc->getWncStatus() == WNC_GOOD )? 0 : -1;
}

//
// ok, the user wants to disconnect.  At present, this isn't possible
// with the WNC, so just fake it and say we did...
//
int WNCInterface::disconnect() {
    return 0;
}

//
// update the networking stats and return the IP Address
//
char * WNCInterface::getIPAddress() {
    M_LOCK;
    if ( _pwnc->getWncNetworkingStats(&myNetStats) ) {
        CHK_WNCFE(( WNCInterface::_pwnc->getWncStatus() == FATAL_FLAG ), null);
        M_ULOCK;
        return &myNetStats.ip[0];
    }
    M_ULOCK;
    return NULL;
}

//
// update the networking stats and return the Gateway Address
//
char * WNCInterface::getGateway() {
    M_LOCK;
    if ( _pwnc->getWncNetworkingStats(&myNetStats) ) {
        CHK_WNCFE(( WNCInterface::_pwnc->getWncStatus() == FATAL_FLAG ), null);
        M_ULOCK;
        return &WNCInterface::myNetStats.gateway[0];
    }
    M_ULOCK;
    return NULL;
}

//
// update the networking stats and return the Network Mask
//
char * WNCInterface::getNetworkMask() {
    M_LOCK;
    if ( _pwnc->getWncNetworkingStats(&myNetStats) ) {
        CHK_WNCFE(( WNCInterface::_pwnc->getWncStatus() == FATAL_FLAG ), null);
        M_ULOCK;    
        return &WNCInterface::myNetStats.mask[0];
    }
    M_ULOCK;    
    return NULL;
}

//
// return a pesudo-MAC address created from the ICCID
//
char* WNCInterface::getMACAddress( void ) {
    string str;
    
    M_LOCK;
    if( _pwnc->getICCID(&str) ) {
        CHK_WNCFE(( WNCInterface::_pwnc->getWncStatus() == FATAL_FLAG ), null);
        mac = str.substr(3,20);
        mac[2]=mac[5]=mac[8]=mac[11]=mac[14]=':';
        M_ULOCK;
        return (char*)mac.c_str();
    }
    M_ULOCK;
    return NULL;
}

