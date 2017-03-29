/* -*- c-file-style:"stroustrup"; indent-tabs-mode: nil -*- */
#include "pbpal.h"

#include "pubnub_internal.h"
#include "pubnub_assert.h"
#include "pubnub_log.h"


enum pbpal_resolv_n_connect_result pbpal_resolv_and_connect(pubnub_t *pb)
{
    char const* origin = PUBNUB_ORIGIN_SETTABLE ? pb->origin : PUBNUB_ORIGIN;
    PUBNUB_LOG_TRACE("resolv_and_connect\n");
    if (NULL == pb->pal.socket) {
        pb->pal.socket = &pb->pal.socket_object;
    }

    WATCH_ENUM(pb->options.use_blocking_io);
    if (pb->pal.socket->connect(origin, 80) < 0) {
        PUBNUB_LOG_ERROR("connect failed\n");
        return pbpal_connect_failed;
    }
    /* This has to be done after the connect(), because it sets
       non-blocking, so we must override if we want blocking. */
    pbpal_set_blocking_io(pb);

    return pbpal_connect_success;
}


enum pbpal_resolv_n_connect_result pbpal_check_resolv_and_connect(pubnub_t *pb)
{
    /* Under mbed2, this function should never be called. 
     */
    PUBNUB_ASSERT_OPT(pb == NULL);
    return pbpal_connect_failed;
}


enum pbpal_resolv_n_connect_result pbpal_check_connect(pubnub_t *pb)
{
    return pb->pal.socket->is_connected() ? pbpal_connect_success : 
        pbpal_connect_wouldblock;
}
