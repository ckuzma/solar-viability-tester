/*
    Copyright (c) 2016 Fred Kellerman
 
    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:
 
    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.
 
    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
    THE SOFTWARE.
    
    @file          WncController.h
    @purpose       Controls WNC Cellular Modem
    @version       1.0
    @date          July 2016
    @author        Fred Kellerman
    
    Notes: This code originates from the following mbed repository:
    
    https://developer.mbed.org/teams/Avnet/code/WncControllerLibrary/
*/


#ifndef __WNCCONTROLLER_H_
#define __WNCCONTROLLER_H_

#include <string>
#include <stdint.h>

namespace WncController_fk {
    
using namespace std;

/**
 *  \file WncController.h
 *  \brief This mbed C++ class is for controlling the WNC
 *  Cellular modem via the AT command interface.  This was
 *  developed with respect to version 1.3 of the WNC authored
 *  spec.  This class is only designed to have 1 instantiation
 *  it is also not multi-thread safe.
 */


static const uint8_t  MAX_LEN_IP_STR = 16;         // Length includes room for the extra NULL

/**
 *  \brief  Contains info fields for the WNC Internet Attributes
 */
struct WncIpStats
{
    string wncMAC;
    char ip[MAX_LEN_IP_STR];
    char mask[MAX_LEN_IP_STR];
    char gateway[MAX_LEN_IP_STR];
    char dnsPrimary[MAX_LEN_IP_STR];
    char dnsSecondary[MAX_LEN_IP_STR];
};

class WncController
{
public:
    static const unsigned MAX_NUM_WNC_SOCKETS = 5;  // Max number of simultaneous sockets that the WNC supports
    static const unsigned MAX_POWERUP_TIMEOUT = 60; // How long the powerUp method will try to turn on the WNC Shield
                                                    //  (this is the default if the user does not over-ride on power-up

    // Tracks mode of the WNC Shield hardware
    enum WncState_e {
        WNC_OFF = 0,
        WNC_ON, // This is intended to mean all systems go, including cell link up but socket may not be open
        WNC_ON_NO_CELL_LINK,
        WNC_NO_RESPONSE
    };

    /**
     *  \brief Constructor for UART controlled WNC
     *
     *  \param [in] wnc_uart - Reference to a SerialBuffered object which will
     *  be used as the bus to control the WNC.  apnStr = a text string for
     *  the cellular APN name.
     *
     *  \return None.
     *
     *  \details Adding another way to talk to the WNC, like I2C or USB,
     *  a constructor should be added for each type just like the SerialBuffered
     *  constructor below.  Assumes UART is enabled, setup and ready to go. This
     *  class will read and write to this UART.
     */
    WncController(void);
    
    // WncController( const char * const apnStr, MODSERIAL * wnc_uart, MODSERIAL * debug_uart = NULL);
    
    /**
     *  \brief Used internally but also make public for a user of the Class to interrogate state as well.
     *
     *  \param [in] None.
     *
     *  \return The state of the WNC Modem.
     *
     *  \details None.
     */
    WncState_e getWncStatus(void);
    
    bool setApnName(const char * const apnStr);

    /**
     *  \brief Return signal quality dBm level
     *
     *  \param [in] None.
     *
     *  \return The dBm signal level at the time of the request.
     *
     *  \details This polls (at the time of the call) the cell signal.
     */
    int16_t getDbmRssi(void);
    int16_t get3gBer(void);

    /**
     *  \brief  Power up and down (down not implemented yet)
     *
     *  \param [in] NXP Pins that are critical for the initialization of the WNC Shield.
     *
     *  \return true if request successful else false.
     *
     *  \details Power-on works but not power-down.  This will manipulate WNC Shield hardware
     *  and bring it to life.  It will also initialize the WNC enough to get it to be able to open sockets
     *  (with AT commands)
     */
    bool powerWncOn(const char * const apn, uint8_t powerUpTimeoutSecs = MAX_POWERUP_TIMEOUT);

    /**
     *  \brief  Query the WNC modem for its Internet attributes
     *
     *  \param [in] Pointer to a struct where to put the info.
     *
     *  \return true if request successful else false.
     *
     *  \details This method will do a few sanity checks and then gather the
     *  fields of the struct.
     */
    bool getWncNetworkingStats(WncIpStats * s);

    /**
     *  \brief Look-up a URL text string and convert into an IP Address string.
     *
     *  \param [in] url - the URL to lookup.  numSock - the socket reference.
     *
     *  \return true - if the IP address has been resolved. false - if the URL could not be resolved.
     *
     *  \details None.
     */
    bool resolveUrl(uint16_t numSock, const char * url);

    /**
     *  \brief Set IP Address string
     *
     *  \param [in] numSock - socket reference to set the string for. ipStr - text string of the IP
     *  address you want to talk to.  There is no sanity check - beware!!!
     *
     *  \return true - if the IP address has been set. false - if the IP could not be set.
     *
     *  \details None.
     */
    bool setIpAddr(uint16_t numSock, const char * ipStr);

    /**
     *  \brief Opens a WNC socket.
     *
     *  \param [in] sockNum - the number of the socket to open.  ipAddr - a string containing
     *  the IP address.  port - the IP port number to open the socket connection.
     *
     *  \return true - if the socket is/was opened.  false otherwise.
     *
     *  \details None.
     */
    bool openSocket(uint16_t numSock, uint16_t port, bool tcp, uint16_t timeOutSec = 30);
    
    bool openSocketUrl(uint16_t numSock, const char * url, uint16_t port, bool tcp, uint16_t timeOutSec = 30);

    bool openSocketIpAddr(uint16_t numSock, const char * ipAddr, uint16_t port, bool tcp, uint16_t timeOutSec = 30);


    /**
     *  \brief Write bytes of data to an open socket
     *
     *  \param [in] sockNum - the number of the socket to write.  s - a string containing
     *  the byte data to send.
     *
     *  \return true - if the write was successful.  false otherwise.
     *
     *  \details The results of the write do not have anything to do with the data
     *  arriving at the endpoint.
     */
     bool write(uint16_t numSock, const char * s, uint32_t n);

    /**
     *  \brief Poll and read back data from the WNC (if it has any)
     *  If auto poll is enabled this read might fail (return with no data).
     *
     *  \param [in] sockNum - the number of the socket to read.  result - a string pointer containing
     *  the byte data readback from the WNC.
     *
     *  \return The number of bytes/chars that are read from the socket.
     *
     *  \details DO NOT use the same string as is passed to the auto poll setup method!
     */
    size_t read(uint16_t numSock, uint8_t * readBuf, uint32_t maxReadBufLen);
    
    size_t read(uint16_t numSock, const uint8_t ** readBuf);

    /**
     *  \brief Set how many times the above read method will retry if data is not returned.
     *
     *  \param [in] sockNum - the number of the socket to set.  retries - how many times to
     *  poll until data is found.
     *
     *  \return None.
     *
     *  \details None.
     */
    void setReadRetries(uint16_t numSock, uint16_t retries);

    /**
     *  \brief Set how long between retries to wait.
     *
     *  \param [in] sockNum - the number of the socket to set.  waitMs - how long to wait
     *  before doing the read poll (calling read(...)).
     *
     *  \return None.
     *
     *  \details None.
     */
    void setReadRetryWait(uint16_t numSock, uint16_t waitMs);

    /**
     *  \brief Close the socket.
     *
     *  \param [in] sockNum - the number of the socket to close.
     *
     *  \return None.
     *
     *  \details None.
     */
    bool closeSocket(uint16_t numSock);

    void setWncCmdTimeout(uint16_t toMs);
        
    bool getIpAddr(uint16_t numSock, char myIpAddr[MAX_LEN_IP_STR]);
    
    void enableDebug(bool on, bool moreDebugOn);
    
    ///////////////////////////////////////////
    //  SMS messaging
    ///////////////////////////////////////////

    static const uint16_t MAX_WNC_SMS_MSG_SLOTS = 3;   // How many SMS messages the WNC can store and receive at a time.
    static const uint16_t MAX_WNC_SMS_LENGTH    = 160; // The maximum length of a 7-bit SMS message the WNC can send and receive.
    
    struct WncSmsInfo
    {
        // Content
        char   idx;
        string number;
        string date;
        string time;
        string msg;
        
        // Attributes
        bool incoming;
        bool unsent;
        bool unread;
        bool pduMode;
        bool msgReceipt;
    };

    struct WncSmsList
    {
        uint8_t msgCount;
        WncSmsInfo e[MAX_WNC_SMS_MSG_SLOTS];
    };

    bool sendSMSText(const char * const phoneNum, const char * const text);

    bool readSMSLog(struct WncSmsList * log);

    bool readUnreadSMSText(struct WncSmsList * w, bool deleteRead = true);
    
    bool saveSMSText(const char * const phoneNum, const char * const text, char * msgIdx);
    
    bool sendSMSTextFromMem(char msgIdx);

    bool deleteSMSTextFromMem(char msgIdx);
    
    bool getICCID(string * iccid);

    bool convertICCIDtoMSISDN(const string & iccid, string * msisdn);
    
    ///////////////////////////////////////////
    // Neighborhood Cell Info
    ///////////////////////////////////////////
    size_t getSignalQuality(const char ** log);
    
    //  Date Time
    struct WncDateTime
    {
        uint8_t  year;
        uint8_t  month;
        uint8_t  day;
        uint8_t  hour;
        uint8_t  min;
        uint8_t  sec;
    };

    bool getTimeDate(struct WncDateTime * tod);
    
    // Ping
    bool pingUrl(const char * url);
    bool pingIp(const char * ip);
    
    // User command:
    size_t sendCustomCmd(const char * cmd, char * resp, size_t sizeRespBuf, int ms_timeout);

protected:

    // Debug output methods
    int dbgPutsNoTime(const char * s, bool crlf = true);
    int dbgPuts(const char * s, bool crlf = true);
    const char * _to_string(int64_t value);
    const char * _to_hex_string(uint8_t value);    

    // Sends commands to WNC via
    enum AtCmdErr_e {
        WNC_AT_CMD_OK,
        WNC_AT_CMD_ERR,
        WNC_AT_CMD_ERREXT,
        WNC_AT_CMD_ERRCME,
        WNC_AT_CMD_INVALID_RESPONSE,
        WNC_AT_CMD_TIMEOUT,
        WNC_AT_CMD_NO_CELL_LINK,
        WNC_AT_CMD_WNC_NOT_ON
    };

    // Users must define these functionalities:
    virtual int putc(char c)              = 0;
    virtual int puts(const char * s)      = 0;
    virtual char getc(void)               = 0;
    virtual int charReady(void)           = 0;
    virtual int dbgWriteChar(char b)      = 0;
    virtual int dbgWriteChars(const char *b) = 0;
    virtual void waitMs(int t)            = 0;
    virtual void waitUs(int t)            = 0;
    virtual bool initWncModem(uint8_t powerUpTimeoutSecs) = 0;
    
    // Isolate OS timers
    virtual int  getLogTimerTicks(void)  = 0;
    virtual void startTimerA(void)       = 0;
    virtual void stopTimerA(void)        = 0;
    virtual int  getTimerTicksA_mS(void) = 0;
    virtual void startTimerB(void)       = 0;
    virtual void stopTimerB(void)        = 0;
    virtual int  getTimerTicksB_mS(void) = 0;
        
    bool waitForPowerOnModemToRespond(uint8_t powerUpTimeoutSecs);    
    AtCmdErr_e sendWncCmd(const char * const s, string ** r, int ms_timeout);
    
private:

    bool softwareInitMdm(void);
    bool checkCellLink(void);
    AtCmdErr_e mdmSendAtCmdRsp(const char * cmd, int timeout_ms, string * rsp, bool crLf = true);
    size_t mdmGetline(string * buff, int timeout_ms);
    bool at_at_wnc(void);
    bool at_init_wnc(bool hardReset = false);
    int16_t at_sockopen_wnc(const char * const ip, uint16_t port, uint16_t numSock, bool tcp, uint16_t timeOutSec);
    bool at_sockclose_wnc(uint16_t numSock);
    bool at_dnsresolve_wnc(const char * s, string * ipStr);
    AtCmdErr_e at_sockwrite_wnc(const char * s, uint16_t n, uint16_t numSock, bool isTcp);
    AtCmdErr_e at_sockread_wnc(uint8_t * pS, uint16_t * numRead, uint16_t n, uint16_t numSock, bool isTcp);
    AtCmdErr_e at_sockread_wnc(string * pS, uint16_t numSock, bool isTcp);
    bool at_reinitialize_mdm(void);
    AtCmdErr_e at_send_wnc_cmd(const char * s, string ** r, int ms_timeout);
    bool at_setapn_wnc(const char * const apnStr);
    bool at_sendSMStext_wnc(const char * const phoneNum, const char * const text);
    bool at_get_wnc_net_stats(WncIpStats * s);
    bool at_readSMSlog_wnc(string ** log);
    size_t at_readSMStext_wnc(const char ** log);
    size_t at_readSMStext_wnc(const char n, const char ** log);
    bool at_getrssiber_wnc(int16_t * dBm, int16_t * ber3g);
    void closeOpenSocket(uint16_t numSock);
    bool sockWrite(const char * const s, uint16_t n, uint16_t numSock, bool isTcp);
    bool at_sendSMStextMem_wnc(char n);
    bool at_deleteSMSTextFromMem_wnc(char n);
    bool at_saveSMStext_wnc(const char * const phoneNum, const char * const text, char * msgIdx);
    size_t at_getSignalQuality_wnc(const char ** log);
    bool at_gettimedate_wnc(struct WncDateTime * tod);
    bool at_ping_wnc(const char * ip);
    bool at_geticcid_wnc(string * iccid);
    
    // Utility methods
    void sendCmd(const char * cmd, bool crLf);
    void sendCmd(const char * cmd, unsigned n, unsigned wait_uS, bool crLf);    
    inline void rx_char_wait(void) {
        // waitUs(1000);
    }
    
    // Important constants
    static const uint16_t MAX_WNC_READ_BYTES        = 1500;                            // This bounds the largest amount of data that the WNC read from a socket will return
    static const uint16_t MAX_WNC_WRITE_BYTES       = MAX_WNC_READ_BYTES;              // This is the largest amount of data that the WNC can write per sockwrite.
    static const uint16_t MAX_LEN_WNC_CMD_RESPONSE  = (MAX_WNC_READ_BYTES * 2 + 100);  // Max number of text characters in a WNC AT response *2 because bytes are converted into 2 hex-digits +100 for other AT@ chars.
    static const uint16_t WNC_AUTO_POLL_MS          = 250;   // Sets default (may be overriden with method) poll interval (currently not used, future possible feature.
    static const uint16_t WNC_CMD_TIMEOUT_MS        = 40000; // Sets default (may be overriden) time that the software waits for an AT response from the WNC.
    static const uint16_t WNC_QUICK_CMD_TIMEOUT_MS  = 2000;  // Used for simple commands that should immediately respond such as "AT", cmds that are quicker than WNC_CMD_TIMEOUT_MS.
    static const uint16_t WNC_WAIT_FOR_AT_CMD_MS    = 0;     // Wait this much between multiple in a row AT commands to the WNC.
    static const uint16_t WNC_SOFT_INIT_RETRY_COUNT = 10;    // How many times the WNC will be tried to revive if it stops responding.
    static const uint16_t WNC_DNS_RESOLVE_WAIT_MS   = 60000; // How much time to wait for the WNC to respond to a DNS resolve/lookup.
    static const uint16_t WNC_TRUNC_DEBUG_LENGTH    = 80;    // Always make this an even number, how many chars for the debug output before shortening the debug ouput, this is used when moreDebug = false. 
    static const uint16_t WNC_APNSET_TIMEOUT_MS     = 60000; // How long to wait for the WNC to respond to setting the APN string.
    static const uint16_t WNC_PING_CMD_TIMEOUT_MS   = 60000; // Amount of time to wait for the WNC to respond to AT@PINGREQ (with cmd default params for timeout, does not change WNC cmd's timeout) 
    static const int      WNC_REINIT_MAX_TIME_MS    = 60000; // How long to wait for the WNC to reset after it was already up and running after power-up.
    static const uint16_t WNC_SOCK_CLOSE_RETRY_CNT  = 3;     // How many times to try to close the socket if the WNC gives an error.
    static const char * const INVALID_IP_STR;                // Just a string set to an IP address when DNS resolve fails.
        
    struct WncSocketInfo_s {
        int16_t numWncSock;
        bool open;
        string myIpAddressStr;
        uint16_t myPort;
        uint8_t readRetries;
        uint16_t readRetryWaitMs;
        bool isTcp;
        uint16_t timeOutSec;
    };

    static WncSocketInfo_s m_sSock[MAX_NUM_WNC_SOCKETS];
    static const WncSocketInfo_s defaultSockStruct;
    static WncState_e m_sState;
    static uint16_t m_sCmdTimeoutMs;
    static string m_sApnStr;
    static string m_sWncStr;
    static uint8_t m_sPowerUpTimeoutSecs;
    static bool m_sDebugEnabled;
    static bool m_sMoreDebugEnabled;
    static bool m_sCheckNetStatus;
    static bool m_sReadyForSMS;
};

};  // End namespace WncController_fk

#endif