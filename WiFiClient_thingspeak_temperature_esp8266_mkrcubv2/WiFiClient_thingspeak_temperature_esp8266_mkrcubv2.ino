/*
 *  This sketch sends data via HTTP to thingspeak and twitter.
 *  Make sure to update your ssid, password, writeAPIKey, and thingtweetAPIKey
 *  By Marc St. Pierre, Tomoe yoshihara, Tim Kong
 */

#include <ESP8266WiFi.h>

const char* ssid        = "......";
const char* password    = "......";
const char* host        = "api.thingspeak.com";
const int httpPort      = 80;
String writeAPIKey      = ".................";
String thingtweetAPIKey = ".................";
const int sensorPin     = A0; //ADC pin connected to an analog sensor
int temperatureCheck[]  = {0, 0}; //this list allows you to compare before and after values
int timePassed          = 0;
int tweettimePassed     = 15; //offset time intervals to ensure that we don't send the tweet and sensor data at the same time
WiFiClient client;

void setup() {
  Serial.begin(115200);
  delay(10);

  // We start by connecting to a WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  delay(10);
  temperatureCheck[0] = tempRead(); //stores an initial temperature reading on startup
}

void loop() { 
  delay(1000);
  temperatureCheck[1] = tempRead();
  timePassed++;
  tweettimePassed++;
  
  // wait till 5 minutes has passed before sending to the server
  if(timePassed >= 300){
    //send data to the server
    sendData(temperatureCheck[1]);
    timePassed = 0;    
  }
  
  //we check every hour to see if the temperature has changed. If so, we tweet it
  if(tweettimePassed >= 3600){
    if(temperatureCheck[0] < temperatureCheck[1]){
      String notification = "Hotter from: ";
      notification       += String(temperatureCheck[0]);
      notification       += " to: ";
      notification       += String(temperatureCheck[1]);
      notification       += " degrees Celcius.";
      updateTwitterStatus(notification);
      Serial.println("Hot notification sent");
      
    }else if(temperatureCheck[0] > temperatureCheck[1]){
      String notification = "Colder from: ";
      notification       += String(temperatureCheck[0]);
      notification       += " to: ";
      notification       += String(temperatureCheck[1]);
      notification       += " degrees Celcius.";
      updateTwitterStatus(notification);
      Serial.println("Cold notification sent");
      
    }else{
      Serial.println("Value unchanged");
    }
    // reset this to count each of the seconds until it hits the time limit again
    tweettimePassed = 0;
    Serial.println("");
    Serial.print("Temperature before: ");
    Serial.print(temperatureCheck[0]);
    Serial.print(" after: ");
    Serial.println(temperatureCheck[1]);
    temperatureCheck[0] = temperatureCheck[1];
  }
}

int tempRead() {
  //reads the value on the sensor and converts it to degrees Celcius
  float temperatureVoltageVal = 0.0;
  int temperatureDegCelcVal   = 0;
  
  //read and store the sensor value
  float sensorVal = analogRead(sensorPin);
//  Serial.print("sensorVal: ");
//  Serial.print(sensorVal);

  //convert raw value to a voltage reading
  temperatureVoltageVal=(sensorVal / 1024) * 5.0;
//  Serial.print("        temperatureVoltageVal: ");
//  Serial.print(temperatureVoltageVal);

  //convert voltage to deg Cel
  temperatureDegCelcVal=(temperatureVoltageVal-0.5) * 10;
  Serial.print("TemperatureDegCelcVal: ");
  Serial.println(temperatureDegCelcVal);
  return temperatureDegCelcVal;
}


//this function sends data directly to THINGSPEAK and logs it on their server
void sendData(int sensorVal){
  Serial.println("Sending data to server now, biatch!...");
  Serial.print("connecting to ");
  Serial.println(host);
  
  // Use WiFiClient class to create TCP connections
  if (client.connect(host, httpPort)) {
    Serial.println("current reading: ");
    Serial.println(sensorVal);
    Serial.println("sending data...");
    
    String postStr = writeAPIKey;
           postStr +="&field1=";
           postStr += String(sensorVal);
    
    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: "+writeAPIKey+"\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(postStr.length());
    client.print("\n\n");
    client.print(postStr);
  }else{
    Serial.println("connection failed");
    return;
  }
  delay(10);
  
  // Read all the lines of the reply from server and print them to Serial
  while(client.available()){
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }
  Serial.println();
  Serial.println("closing thingspeak connection");
}


//this function sends data to TWITTER via thingspeak to update a status
void updateTwitterStatus(String tsData){
  Serial.println("Sending notification to twitter now...");
  if (client.connect(host, httpPort)){ 
    // Create HTTP POST Data
    tsData = "api_key="+thingtweetAPIKey+"&status="+tsData;       
    client.print("POST /apps/thingtweet/1/statuses/update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(tsData.length());
    client.print("\n\n");
    
    client.print(tsData);
    }else{
    Serial.println("connection failed");
    return;
  }
  delay(10);
    
  // Read all the lines of the reply from server and print them to Serial
  while(client.available()){
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }
  Serial.println();
  Serial.println("closing twitter connection");
}
