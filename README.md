# solar-viability-tester

![Main project image](https://github.com/ckuzma/solar-viability-tester/blob/master/photos/project_image.jpg)

## Introduction
This repo holds the code for Chris Kuzma's [project submission](https://www.hackster.io/kuzma/solar-viability-tester-6cb5f1) for the [Realtime AT&T IoT StarterKit challenge](https://www.hackster.io/contests/RealtimeAT&TIoTStarterKit) contest hosted by [Hackster.io](https://www.hackster.io/) and [AT&T](https://www.att.com/). The goal of the project is to build a wireless, self-powered and contained, cloud-enabled test for determining the viability of a permanent solar cell array installation for a given site.

## Live Results (Last Updated 4/29/2017)
Archived results from an installation in New York City can be found here:

[Solar Viability Tester - Archived Results](https://github.com/ckuzma/solar-viability-tester/blob/master/data.csv)

If it's cloudy outside, then there isn't enough power to keep both the modem and the NXP board turned on. Instead they just power cycle again and again repeatedly like so:

![Power cycling board](https://github.com/ckuzma/solar-viability-tester/blob/master/photos/k64f_power_flash.gif)

## Hardware
### [NXP FRDM-K64F MCU](https://developer.mbed.org/platforms/FRDM-K64F/)
![NXP FRDM-K64F MCU](https://github.com/ckuzma/solar-viability-tester/blob/master/photos/frdm.jpg)

### [Avnet M14A2A Cellular Shield](http://cloudconnectkits.org/product/att-cellular-iot-starter-kit)
![Avnet M14A2A Cellular Shield](https://github.com/ckuzma/solar-viability-tester/blob/master/photos/wnc_modem.jpg)

### [Allpowers 15-watt 5-volt Solar Panel](https://www.amazon.com/ALLPOWERS-Portable-Foldable-Technology-Sunpower/dp/B0188OTGYA)
![Whole kit laid on top of solar panel](https://github.com/ckuzma/solar-viability-tester/blob/master/photos/all_hardware.jpg)

## How-To / Setup
These instructions expect a basic familiarity with the AT&T PubNub platform, which was designed to connect IoT devices to other services and applications, and IBM Bluemix, which hosts our Python Flask webserver and PubNub event listener. You will need accounts for both services before beginning. Both have free tiers allowing anyone to replicate this project on a non-commercial scale.

Having created a PubNub account, one needs to then create a new app from the main PubNub dashboard, along with a new keyset to go along with it. The Publish and Subscribe keys are what we're after... they'll allow our AT&T IoT Starter Kit to publish messages, and our Python app to listen for and capture them. Once you have them, you'll want to drop them into the FRDM-K64F program on lines 9 and 10 of the [main.cpp](https://github.com/ckuzma/solar-viability-tester/blob/master/mbed/Solar_Viability_Tester/main.cpp) file:

```
// Define these user values!
const char *pubKey = "demo"; // PubNub publish key
const char *subKey = "demo"; // PubNub subscribe key
char *channel = "hello_world"; // Channel name
int timeDelay = 600; // 600 seconds (10 minutes)
```

At this point you may as well give a name to your channel, though you can leave it as-is if you'd like. (Note: The code will actually compile and run as-is, but messages will be published to [the demo channel](https://www.pubnub.com/docs/console?channel=hello_world&origin=pubsub.pubnub.com&sub=demo&pub=demo) which anyone else in the world can also post messages to.) You'll then need to import the entire mbed part of the project into the online mbed IDE in order to compile it unless you plan on compiling it locally with your own setup.

Having compiled the firmware you'll be presented with a .bin binary download. You'll transfer this to the FRDM-K64F board in just the same was as the hello_world example in the board's quickstart guide. If your board isn't mounting correctly or accepting code, then you may need to run the fix described [here](https://www.hackster.io/kuzma/solar-viability-tester-6cb5f1#toc-fix-for-frdm-board-not-mounting---accepting-code-8).

With that complete you'll now want to deploy the webserver to Bluemix. First, on your local machine, copy the creds.json.example file and rename it creds.json. Then in your favorite editor drop in the PubNub keys that you acquired earlier. Now you're ready to deploy! Modify all instances of the string "solar-viability-tester" in these three files (`bluemix-deploy-solar-viability-tester.sh`, `manifest.yml`, and `setup.py`) to whatever you'd like as a subdomain, and then you'll be ready to deploy to Bluemix.

If you've never deployed an app to Bluemix before you'll first need to install the command-line tools. It just so happens that there's [a handy guide right here](https://console.ng.bluemix.net/docs/cli/index.html) that tells you how to do exactly that. Once you've got the tools installed, just follow these instructions from a terminal opened to the root directory of the project repo:

```
cf api https://api.ng.bluemix.net
cf login -u <email_login> -o <email_login>
cf target -s dev
cf <your_subdomain>
```

And that's it! Now go attach your AT&T IoT Starter Kit to a solar panel and occasionally refresh the website you created (<your_domain>.mybluemix.net) to see how much power you've generated each day.
