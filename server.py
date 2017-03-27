## Import blackboxes
import cf_deployment_tracker
import json
import os
from flask import Flask
from flask_restful import Resource, Api

## Import our stuff
from api.pubnub import PubNubApi

# Emit Bluemix deployment event
cf_deployment_tracker.track()

## Configure Flask server
port = int(os.getenv('VCAP_APP_PORT', 8080))
app = Flask(__name__)
api = Api(app)

class HelloWorld(Resource):
    def get(self):
        return {'hello': 'world'}

api.add_resource(HelloWorld, '/')

if __name__ == '__main__':
    ## Read in credentials
    raw = open('creds.json', 'r')
    keys = json.loads(raw.read())
    raw.close()

    ## Initialize the API
    api = PubNubApi(keys['publish'], keys['subscribe'])

    ## Begin listening for messages
    api.subscribe('awesomeTest')
    print('Listening...')

    ## Start the server
    app.run(host='0.0.0.0', port=port, debug=False)
