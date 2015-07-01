#WiFi Client Tutorial
Log ESP temperature data to thingspeak.com

- Build your esp circuit in boot mode (see previous tutorial) with GPIO0 connected to ground
- Connect the ADC pin to the analog output of a TMP36 sensor through a 4.7kohm/10kohm voltage divider
- Create an account on thingspeak
- Create a channel and make a note of the apiKey provided
- Using the sketch provided in this repo, modify the settings to match your wifi network and specific thingspeak channel key
- Upload the sketch and open the serial monitor for debugging

you should now be logging raw temperature data every 30 seconds

#### Notes ####
* the ADC pin can only read values between 0 and 1 volts, so output voltage from a sensor must be stepped down by 4.7k first.
* the TMP36 sensor ground pin connects directly to ground while its middle pin connects to 4.7k R
* thingspeak can only accept entries >=15s apart. If the entries are too frequent they will be dropped.
