/* -*- c-file-style:"stroustrup"; indent-tabs-mode: nil -*- */
#if !defined INC_PUBNUB_INTERNAL
#define      INC_PUBNUB_INTERNAL


/* This is support for mbed 2 socket interface.
   mbed 5 has a significantly different socket interface.
 */

#include <TCPSocketConnection.h>


typedef TCPSocketConnection* pb_socket_t;

#define socket_close(socket) (socket)->close()
#define socket_send(socket, buf, len, flags) (socket)->send((buf), (len))
#define socket_recv(socket, buf, len, flags) (socket)->receive((buf), (len))

#define socket_would_block() 0

#define socket_timed_out() 0

#define socket_platform_init() 0

#define SOCKET_INVALID NULL
#define SOCKET_ERROR -1

#define socket_is_connected(socket) (socket)->is_connected()

#define socket_set_rcv_timeout(socket, milliseconds) int dont_call_me_under_mbed_2[-1]


/** The Pubnub mbed 2 context */
struct pubnub_pal {
    /** This is actually just a pointer. We need it to emulate the 
        BSD sockets for which socket is a handle (a sort of a pointer)
        and you can write some value into it, like #SOCKET_INVALID.
     */
    pb_socket_t socket;

    /** This is the real socket object, to which the #socket 
        points to when it's valid.
     */
    TCPSocketConnection socket_object;
};


/** On mbed2, one can set I/O to be blocking or non-blocking, though
    how that actually works depends on the actual hardware and the
    "driver".
 */
#define PUBNUB_BLOCKING_IO_SETTABLE 1


#define PUBNUB_TIMERS_API 1



#include "pubnub_internal_common.h"



#endif /* !defined INC_PUBNUB_INTERNAL */
