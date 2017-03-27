## Import blackboxes
from pubnub.callbacks import SubscribeCallback
from pubnub.enums import PNOperationType, PNStatusCategory
from pubnub.pnconfiguration import PNConfiguration
from pubnub.pubnub import PubNub, NonSubscribeListener

## Import our stuff
from utils.datawriter import DataWriter

listener = NonSubscribeListener()

class MySubscribeCallback(SubscribeCallback):
    def __init__(self):
        self.log = DataWriter()

    def status(self, pubnub, status): # handle status changes
        pass

    def presence(self, pubnub, presence): # handle incoming presence data
        pass

    def message(self, pubnub, message): # handle incoming messages
        self.log.write_solar_info(message.message)
        print(message.message)

class PubNubApi:
    def __init__(self, pub_key, sub_key):
        pnconfig = PNConfiguration()
        pnconfig.publish_key = pub_key
        pnconfig.subscribe_key = sub_key
        pnconfig.ssl = False
        self.pubnub = PubNub(pnconfig)
        self.pubnub.add_listener(MySubscribeCallback())

    def publish(self, channel, message):
        self.pubnub.publish().channel(channel).message(message).async(listener.callback)

    def subscribe(self, channel):
        self.pubnub.subscribe().channels(channel).execute()

if __name__ == '__main__':
    print('Running tests for: pubnub-api.py')

    import json
    import sys

    ## Read in credentials
    raw = open('../creds.json', 'r')
    keys = json.loads(raw.read())
    raw.close()

    ## Initialize the API
    api = PubNubApi(keys['publish'], keys['subscribe'])

    ## Set the tests to run
    publish_test = False
    listen_test = False

    ## Test functions
    if publish_test:
        message = {
            'text': sys.argv[1]
        }
        api.publish('awesomeTest', message)
    if listen_test:
        api.subscribe('awesomeTest')
        print('Listening...')
