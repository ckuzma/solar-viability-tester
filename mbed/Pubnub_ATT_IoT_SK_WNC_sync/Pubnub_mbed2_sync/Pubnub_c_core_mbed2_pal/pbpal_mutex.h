/* -*- c-file-style:"stroustrup"; indent-tabs-mode: nil -*- */
#if !defined INC_PBPAL_MUTEX
#define      INC_PBPAL_MUTEX

/** @file pbpal_mutex.h
    PAL for mutex(es) on mbed, using CMSIS-RTOS API.
 */

#include "cmsis_os.h"

struct pbcmsis_mutex {
    osMutexId mutid;
    osMutexDef_t mutdef;
};

typedef struct pbcmsis_mutex pbpal_mutex_t;

#define pbpal_mutex_init(m) do {                                        \
    memset(&((m).mutdef), 0, sizeof (m).mutdef);                        \
    (m).mutid = osMutexCreate((m).mutef);                               \
    } while (0)

#define pbpal_mutex_lock(m) osMutexWait((m).mutid, osWaitForever)
#define pbpal_mutex_unlock(m) osMutexRelease((m).mutid)
#define pbpal_mutex_destroy(m) osMutexDelete((m).mutid)
#define pbpal_mutex_decl_and_init(m) pbpal_mutex_t m = { 0 }; (m).mutid = osMutexCreate(&(m).mutdef)
#define pbpal_mutex_static_decl_and_init(m) static pbpal_mutex_t m = { 0 }
#define pbpal_mutex_init_static(m) (m).mutid = osMutexCreate(&(m).mutdef)


#endif /*!defined INC_PBPAL_MUTEX*/

