#include "mbed.h"
#include "WNCInterface.h"

#include "pubnub_sync.h"
#include "srand_from_pubnub_time.h"


#define CRLF "\r\n"

WNCInterface eth;
MODSERIAL pc(USBTX,USBRX,256,256);


static void generate_uuid(pubnub_t *pbp)
{
    char const *uuid_default = "amzuk-axolotl";
    struct Pubnub_UUID uuid;
    static struct Pubnub_UUID_String str_uuid;

    if (0 != pubnub_generate_uuid_v4_random(&uuid)) {
        pubnub_set_uuid(pbp, uuid_default);
    }
    else {
        str_uuid = pubnub_uuid_to_string(&uuid);
        pubnub_set_uuid(pbp, str_uuid.uuid);
        pc.printf("Generated UUID: %s\n", str_uuid.uuid);
    }
}


int main()
{
    pc.baud(115200);
    pc.printf(CRLF CRLF "Pubnub Test starting..." CRLF);
    
    pc.printf("init() returned 0x%04X" CRLF, eth.init(NULL,&pc));
    eth.connect();
    pc.printf("IP Address: %s" CRLF, eth.getIPAddress());
    eth.doDebug(1);

    
    pubnub_t *pbp = pubnub_alloc();
    pubnub_init(pbp, "demo", "demo");  

    srand_from_pubnub_time(pbp);
    generate_uuid(pbp);
      
    while (true) {
        pubnub_res rslt = pubnub_publish(pbp, "hello_world", "\"Hello world from MBed WNC!\"");
        if (rslt != PNR_STARTED) {
            pc.printf("Failed to start publishing, rslt=%d"CRLF, rslt);
        }
        else {
            rslt = pubnub_await(pbp);
            if (rslt != PNR_OK) {
                pc.printf("Failed to finished publishing, rslt=%d"CRLF, rslt);
            }
            else {
                pc.printf("Published! Response from Pubnub: %s"CRLF, pubnub_last_publish_result(pbp));
            }
        }
        
        rslt = pubnub_subscribe(pbp, "hello_world", 0);
        if (rslt != PNR_STARTED) {
            pc.printf("Failed to start subscribing, rslt=%d"CRLF, rslt);
        }
        else {
            rslt = pubnub_await(pbp);
            if (rslt != PNR_OK) {
                pc.printf("Failed to finished subscribing, rslt=%d"CRLF, rslt);
            }
            else {
                pc.printf("Subscribed! Received messages follow: %s"CRLF);
                while (char const *msg = pubnub_get(pbp)) {
                    pc.printf("subscribe got: %s"CRLF, msg);
                }
            }
        }
        wait_ms(1000);
    }
}

