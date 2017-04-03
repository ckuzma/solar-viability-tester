#include "mbed.h"
#include "WNCInterface.h"
#include "pubnub_sync.h"
#include "srand_from_pubnub_time.h"

#define CRLF "\r\n"

// Define these user values!
const char *pubKey = "demo"; // PubNub publish key
const char *subKey = "demo"; // PubNub subscribe key
char *channel = "hello_world"; // Channel name
int timeDelay = 600; // 600 seconds (10 minutes)

// State function prototypes
void publishMessage(pubnub_t *, char *, char *);
void generate_uuid(pubnub_t *);

// Initialize interfaces
WNCInterface eth;
MODSERIAL pc(USBTX,USBRX,256,256);

int main()
{
    // Connect to serial, modem
    pc.baud(115200);
    pc.printf(CRLF CRLF "Solar viability tester warming up..." CRLF);
    pc.printf("init() returned 0x%04X" CRLF, eth.init(NULL,&pc));
    eth.connect();
    pc.printf("IP Address: %s" CRLF, eth.getIPAddress());
    eth.doDebug(0); // Change to 1 for debug statements

    // Initialize PubNub & UUID
    pubnub_t *pbp = pubnub_alloc();
    pubnub_init(pbp, pubKey, subKey);
    srand_from_pubnub_time(pbp);
    generate_uuid(pbp);

    while (true) { // Endless loop
        char *powerMessage = "{\"power_type\":\"default\"}";
        publishMessage(pbp, channel, powerMessage);

        pc.printf(CRLF"Sleeping for %d seconds..."CRLF, timeDelay);
        wait(timeDelay);
    }
}

void generate_uuid(pubnub_t *pbp)
{
    char const *uuid_default = "att-iot-starter-kit";
    struct Pubnub_UUID uuid;
    static struct Pubnub_UUID_String str_uuid;

    if (0 != pubnub_generate_uuid_v4_random(&uuid)) {
        pubnub_set_uuid(pbp, uuid_default);
    } else {
        str_uuid = pubnub_uuid_to_string(&uuid);
        pubnub_set_uuid(pbp, str_uuid.uuid);
        pc.printf(CRLF"Generated UUID: %s\n", str_uuid.uuid);
    }
}

void publishMessage(pubnub_t *pbp, char *channel, char *message) {
    pc.printf(CRLF"Sending: %s"CRLF,message);
    pubnub_res rslt = pubnub_publish(pbp, channel, message);
    if (rslt != PNR_STARTED) {
        pc.printf(CRLF"Failed to start publishing, rslt=%d"CRLF, rslt);
    } else {
        rslt = pubnub_await(pbp);
        if (rslt != PNR_OK) {
            pc.printf(CRLF"Failed to finished publishing, rslt=%d"CRLF, rslt);
        } else {
            pc.printf(CRLF"Published! Response from Pubnub: %s"CRLF, pubnub_last_publish_result(pbp));
        }
    }
}
