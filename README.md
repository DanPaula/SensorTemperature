# Send data from DHT11 temperature and humidity sensor to Firebase

This project was created with an ESP8266 and a DHT11 temperature and humidity sensor. 
Functionalities:
- connect to wifi automatically; if wifi SSID and password is new or password changed at the press of a button it can open a web page where the user can add this information
- collect data from the sensor and writes it in Firebase
- this data will be then read from an Android device with app created here: https://github.com/DanPaula/SmartHome
