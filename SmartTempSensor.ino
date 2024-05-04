#include <ESP8266WiFi.h>
#include <DHT.h>
#include <ESP8266WebServer.h>
#include <Firebase_ESP_Client.h>
//Provide the token generation process info.
#include "addons/TokenHelper.h"
//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

#define DHTPIN D7
#define DHTTYPE DHT11

const char* ssid = "";
const char* password = "";
#define FIREBASE_AUTH ""
#define FIREBASE_HOST ""
#define USER_EMAIL ""
#define USER_PASSWORD ""

ESP8266WebServer server(80);
DHT dht(DHTPIN, DHTTYPE);

String userId;
String sensorId = "3535ras";
//Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;
//unsigned long sendDataPrevMillis = 0;
//int count = 0;
bool signupOK = false;

void setup() {
  Serial.begin(115200);
  Serial.println();

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to ");
  Serial.print(ssid);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Start HTTP server
  server.on("/user", HTTP_POST, handleUserData);
  server.begin();
  Serial.println("HTTP server started");

  /* Assign the api key (required) */
  config.api_key = FIREBASE_AUTH;

  /* Assign the RTDB URL (required) */
  config.database_url = FIREBASE_HOST;

  // Assign the user sign in credentials
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  Firebase.reconnectWiFi(true);
  fbdo.setResponseSize(4096);


  // Assign the callback function for the long running token generation task
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h

  // Assign the maximum retry of token generation
  config.max_token_generation_retry = 5;

  // Initialize the library with the Firebase authen and config
  Firebase.begin(&config, &auth);
  
  Serial.println("new way");
  // Initialize DHT sensor
  dht.begin();
}

void loop() {

  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();
  server.handleClient();
  String path = "/Users/" + userId + "/Sensors/" + sensorId;

  if (Firebase.ready()) {
    Serial.println(userId.length());
    if (!isnan(humidity) && !isnan(temperature) && userId.length() > 0) {
      Serial.println("first if");
      if (Firebase.RTDB.setFloat(&fbdo, path + "/humidity",humidity)){
       Serial.print("Humidity: ");
       Serial.println(humidity);
      
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }
    
    
    // Write an Float number on the database path test/float
    if (Firebase.RTDB.setFloat(&fbdo, path + "/temperature", temperature)){
       Serial.print("Temperature: ");
       Serial.println(temperature);
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }

    }
  }

  

  delay(5000); // Adjust as needed
}

void handleUserData() {
  if (server.hasArg("userId")) {
    userId = server.arg("userId");
    server.send(200, "text/plain", "User ID received");
  } else {
    server.send(400, "text/plain", "Missing user ID");
  }
}