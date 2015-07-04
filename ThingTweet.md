#ESP -> Thingspeak -> Twitter Tutorial
Post status updates to twitter via thingspeak

- After finishing the WiFi Client tutorial, on your thingspeak account, navigate to Apps and select ThingTWeet
- Link your Twitter Account and copy/paste the API Key into the provided code
- Your device should now tweet the temperature when it detects a change!

#### Notes ####
* when modifying the code, keep in mind to not send a tweet and update the chart at the same time, thingspeak will only receive data in increaments larger than 15 seconds.
* Twitter has an anti spam policy which BLOCKS any exact duplicates of previous tweets, every tweet you send needs to be different