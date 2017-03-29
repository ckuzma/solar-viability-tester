/* -*- c-file-style:"stroustrup"; indent-tabs-mode: nil -*- */
#if !defined INC_PUBNUB_MBED_SYNC
#define	INC_PUBNUB_MBED_SYNC

/** @mainpage The MBed Pubnub client library - sync interface

    This is the "sync" interface of the Pubnub client library for
    MBed. 

    The "sync" interface has these characteristics:
    - There is no "callback" on the end of a transaction. Instead,
    you should check (via pubnub_last_result()) to see if the 
    transaction completed, or if you don't need to do anything
    else on the thread until the transaction is completed (including
    possible thread shutdown), you can call pubnub_await() which
    will do it for you (and return the final result).

    You can have multiple pubnub contexts established; in each
    context, at most one Pubnub API call/transaction may be ongoing
    (typically a "publish" or a "subscribe").
    
 */

/** @file pubnub_sync.h */

#include "pubnub_config.h"

/* -- You should not change anything below this line -- */

#include "pubnub_alloc.h"
#include "pubnub_assert.h"
#include "pubnub_coreapi.h"
#include "pubnub_ntf_sync.h"
#include "pubnub_generate_uuid.h"
#include "pubnub_blocking_io.h"


#endif /* !defined INC_PUBNUB_MBED_SYNC */
