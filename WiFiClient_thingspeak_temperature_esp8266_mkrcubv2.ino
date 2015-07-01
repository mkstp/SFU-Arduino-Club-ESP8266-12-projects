/*
 *  This sketch sends data via HTTP to thingspeak service.
 *  By Marc St. Pierre, Tomoe yoshihara, Tim Kong
 */

#include <ESP8266WiFi.h>

const char* ssid     = "........";
const char* password = ".............";
const char* host = "api.thingspeak.com";
String writeAPIKey = "....................";
const int sensorPin = A0; //ADC pin connected to analog sensor
float temperatureVoltageVal=0.0;
float temperatureDegCelcVal=0.0;

int timePassed = 0;

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
}

void loop() {
  //delay(15000);
  float sensorVal = analogRead(sensorPin); //read and store the sensor value
  delay(1000);
  Serial.print("sensorVal: ");
      Serial.print(sensorVal);
  /*float temp = (535 * .251);
  Serial.print("temp: ");
      Serial.println(temp);*/
  temperatureVoltageVal=(sensorVal / 1024) * 5.0;
      Serial.print("        temperatureVoltageVal: ");
      Serial.print(temperatureVoltageVal);
  temperatureDegCelcVal=(temperatureVoltageVal-0.5) * 10;
        Serial.print("        temperatureDegCelcVal: ");
    Serial.println(temperatureDegCelcVal);
  //sensorVal=temperatureDegCelcVal;
  
  
  
  // wait till 15 seconds has passed before sending to the server
  timePassed++;
  if(timePassed >= 15){
    
    sendData(temperatureDegCelcVal);
    // reset this to count each of the seconds until it hits 15 again
    timePassed = 0;
  }
}


void sendData(int sensorVal){
  Serial.println("Sending data to server now, biatch!...");
  Serial.print("connecting to ");
  Serial.println(host);
  
  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 80;
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
  Serial.println("closing connection");
}
