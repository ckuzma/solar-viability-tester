import json
from time import gmtime, strftime

class DataWriter:
    def __init__(self):
        self.filename = 'data_log.csv'

    def write(self, data_as_string):
        raw = open(self.filename, 'a')
        raw.write(data_as_string + '\n')
        raw.close()

    def write_solar_info(self, data):
        timestamp = strftime("%Y-%m-%d %H:%M:%S", gmtime())
        ping_type = data['type']
        self.write(timestamp + ',' + ping_type)
