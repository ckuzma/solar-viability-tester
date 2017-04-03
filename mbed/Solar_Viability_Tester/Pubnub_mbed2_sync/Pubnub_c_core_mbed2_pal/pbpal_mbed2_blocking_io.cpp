/* -*- c-file-style:"stroustrup"; indent-tabs-mode: nil -*- */
#include "pbpal.h"

#include "pubnub_internal.h"


int pbpal_set_blocking_io(pubnub_t *pb)
{
    pb->pal.socket->set_blocking(pb->options.use_blocking_io, pb->transaction_timeout_ms);
    return 0;
}
