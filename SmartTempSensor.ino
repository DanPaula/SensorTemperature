#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <DHT.h>
#include <FirebaseESP8266.h>

#define API_KEY "AIzaSyC-Baf7wvOy79tHrPdE06aotdUXtR-eXf8"
#define DATABASE_URL "https://smarthome-a51d2-default-rtdb.firebaseio.com/"

#define TRIGGER_PIN D2

#define DHTTYPE DHT11   // DHT 11

// DHT Sensor
uint8_t DHTPin = D7; 
// Initialize DHT sensor.
DHT dht(DHTPin, DHTTYPE);                

float Temperature;
float Humidity;

FirebaseAuth auth;
FirebaseData fbdo;
FirebaseConfig config;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("\n Starting...");
  pinMode(TRIGGER_PIN, INPUT);

  pinMode(DHTPin, INPUT);
  dht.begin();    
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;  
  Firebase.begin(&config, &auth);        
}

void loop() {
  float h = dht.readHumidity();                               
  float t = dht.readTemperature();
  // put your main code here, to run repeatedly:
  if(digitalRead(TRIGGER_PIN) == LOW)
  {
    WiFiManager wifiManager;
    if(!wifiManager.startConfigPortal("OnDemand High Volatges"))
    {
      Serial.println("failed to connect and hit timeout");
      delay(3000);
      ESP.reset();
      delay(5000);
    }
    Serial.println("connected...");
  }

  if (isnan(h) || isnan(t))                                     // Checking sensor working
  {                                   
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  } 
  Serial.print("Humidity: ");  
  Serial.print(h);
  
  Serial.print("%  Temperature: ");  
  Serial.print(t);  
  Serial.println("°C ");
  delay(5000);
 
 
  Firebase.setFloat("/326f1a/Humidity", h);            //setup path to send Humidity readings
  Firebase.setFloat("/326f1a/Temperature", t);         //setup path to send Temperature readings
  if (Firebase.failed()) 
  {
    Serial.print("pushing /logs failed:");
    Serial.println(Firebase.error()); 
    return;
  }
}
