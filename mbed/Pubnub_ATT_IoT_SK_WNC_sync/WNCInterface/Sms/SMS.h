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

    @file          SMS.h
    @version       1.0
    @date          Oct 2016

======================================================================== */

#ifndef SMS_H_
#define SMS_H_

/** Virtual calss for IoT Short Messages Service (SMS) **/

class SMSInterface {
public:
  /** Initialize interface
   */
  virtual int init() = 0;


  /** Send a SM
     @param number The receiver's contact number
     @param message The message to send
     @return 0 on success, error code on failure
   */
  virtual int send(const string& number, const string& message) = 0;


  /** Receive a SM
     @param msg_orig string containing the sender's phone number
     @param Pointer to a string that contains the incoming message
     @return 0 on success, error code on failure
   */
  virtual int get(string& number, string& message) = 0;
};

#endif /* SMS_H_ */
