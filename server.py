import json

from flask import Flask
from pubnub.callbacks import SubscribeCallback
from pubnub.pnconfiguration import PNConfiguration
from pubnub.pubnub import PubNub
from time import localtime, strftime

## Fill in with your values:
PANEL_AMPS = 3.0
PANEL_WATTS = 15.0
CSV_FILE = './data.csv'
CREDS_FILE = './creds.json'
HTML_FOOTER = '<h4><p>Created by <a href=\"http://christopherkuzma.com/\">Christopher Kuzma</a></p></h4>'

## Read in credentials
raw = open(CREDS_FILE, 'r')
keys = json.loads(raw.read())
raw.close()

class MySubscribeCallback(SubscribeCallback):
    def __init__(self):
        self.solar = SolarViability()

    def message(self, pubnub, message):
        print('New ping from solar panel!')
        string_json = json.dumps(message.message)
        self.solar.save_to_csv(string_json)

class SolarViability:
    def _get_date_time(self):
        """
        Returns a list.
        x[0] = date
        x[1] = time
        """
        timestamp = strftime("%Y-%m-%d %H:%M:%S", localtime())
        return timestamp.split(' ')

    def save_to_csv(self, data_string):
        """
        Takes in a string and appends it to CSV_STRING in
        this following format:

        YY-MM-DD,HH:MM:SS,data_string
        """
        csv_string_to_add = self._get_date_time()
        csv_string_to_add.append(data_string)
        csv_string_to_add = ','.join(csv_string_to_add) + '\n'
        print('Saving to file:')
        print(csv_string_to_add)
        print('')

        raw = open(CSV_FILE, 'a')
        raw.write(csv_string_to_add)
        raw.close()

    def get_power_per_day(self):
        """
        Reads in the data CSV string and returns the amount
        of power generated per day as a newline separated
        string that looks like this:

        2017-04-01 = X mW
        """
        raw = open(CSV_FILE)
        data = raw.read()
        raw.close()

        data = data.split('\n')
        day_power = {}
        x = 0
        while x < len(data):
            csv_line = data[x]
            csv_line = csv_line.split(',')
            if len(csv_line) == 3:
                if csv_line[0] in day_power:
                    day_power[csv_line[0]] += 1
                else:
                    day_power[csv_line[0]] = 1
            x+=1
        power_per_day = []
        for key,value in day_power.items():
            """
            Because each ping received accounts for 10 minutes, our per-hour
            calculations have to divide everything by 6 before multiplying
            the solar panel's Watts / Amps rating.
            """
            mWh = str((1.0/6.0) * value * PANEL_WATTS)
            mAh = str((1.0/6.0) * value * PANEL_AMPS)
            power_per_day.append(key + ' = ' + mWh + ' wH / ' + mAh + ' aH')
        return '\n<br>\n'.join(power_per_day)


app = Flask(__name__)
solar = SolarViability()

@app.route('/')
def root():
    page_html = """
    <p><h1>Solar Viability Tester</h1></p>
    <h3><p>
    This page shows the electricity generated
    <br>
    per-day by a 15-watt (3-amp) solar panel
    <br>
    installed in New York City. Values are in
    <br>
    watt-hours and amp-hours. For more
    <br>
    information, see the <a href=\"https://www.hackster.io/kuzma/solar-viability-tester-6cb5f1\">project page on
    <br>
    Hackster</a> or the <a href=\"https://github.com/ckuzma/solar-viability-tester\">GitHub repo</a>.
    </p>
    <p>
    Results updated every 10 minutes.
    </p></h3>
    """
    results = '<p>' + solar.get_power_per_day() + '</p>'
    return page_html + results + HTML_FOOTER

if __name__ == '__main__':
    ## Initialize pubnub
    pnconfig = PNConfiguration()
    pnconfig.publish_key = keys['publish']
    pnconfig.subscribe_key = keys['subscribe']
    pubnub = PubNub(pnconfig)

    ## Start the PubNub listener
    print('Starting PubNub listener...')
    pubnub.add_listener(MySubscribeCallback())
    pubnub.subscribe().channels('solar_viability').execute()

    ## Start the server
    print('Starting server...')
    app.run()
