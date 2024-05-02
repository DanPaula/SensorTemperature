#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <DHT.h>
#include <ESP8266HTTPClient.h>

#define FIREBASE_AUTH ""
#define FIREBASE_HOST ""

#define TRIGGER_PIN D2

#define DHTTYPE DHT11   // DHT 11

#define WIFI_SSID ""
#define WIFI_PASSWORD ""
// DHT Sensor
uint8_t DHTPin = D7; 
// Initialize DHT sensor.
DHT dht(DHTPin, DHTTYPE);     

ESP8266WebServer server(80);

float Temperature;
float Humidity;

const char* firebaseHost = "";
const char* firebasePath = "";

String tempPath = "/temperature";
String humPath = "/humidity";
String userId;
String sensorId = "3535ras";

WiFiClient client;
String uid;
String databasePath; //saves database main path, which will be updated to user UID 
unsigned long sendDataPrevMillis = 0;
bool signupOK = false;

// Initialize WiFi
void initWiFi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
  Serial.println();
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("\n Starting...");

  initWiFi();
  pinMode(DHTPin, INPUT);
  dht.begin();  

 // Start HTTP server
  server.on("/user", HTTP_POST, handleUserData);
  server.on("/sensor", handleSensorData);
  server.begin();
  Serial.println("HTTP server started");  
}

void loop() {
  
  float h = dht.readHumidity();                               
  float t = dht.readTemperature();

  server.handleClient();

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

  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    WiFiClient client; // Create WiFiClient object

    String firebasePathTemperature = "/Users" + userId + "/Sensors" + sensorId + "/temperature";
    String firebasePathHumidity = "/Users" + userId + "/Sensors" + sensorId + "/humidity";
    // Construct Firebase URL
    String urlTemperature = "https://" + String(firebaseHost) + String(firebasePathTemperature);
    String urlHumidity = "https://" + String(firebaseHost) + String(firebasePathHumidity);

    // Send POST request with sensor data
    http.begin(client, urlTemperature);
    http.addHeader("Content-Type", "application/json");
    int httpResponseCodeT = http.POST(String(t));

    if (httpResponseCodeT > 0) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCodeT);
    } else {
      Serial.print("Error in HTTP request: ");
      Serial.println(httpResponseCodeT);
    }

    http.begin(client, urlHumidity);
    http.addHeader("Content-Type", "application/json");
    int httpResponseCodeH = http.POST(String(h));

    if (httpResponseCodeH > 0) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCodeH);
    } else {
      Serial.print("Error in HTTP request: ");
      Serial.println(httpResponseCodeH);
    }


    http.end();
  }

  delay(5000); // Send data every 5 seconds
}

void handleUserData() {
  userId = server.arg("user");
  Serial.println("Received user ID: " + userId);
  server.send(200, "text/plain", "User ID received");
}

void handleSensorData() {
  if (userId.length() > 0) {
    Serial.println("Sending sensor ID: " + sensorId);
    server.send(200, "text/plain", sensorId);
  } else {
    server.send(400, "text/plain", "User ID not received");
  }
}
