#include "ESP8266WiFi.h"
#include "ESP8266HTTPClient.h"
#include "BlynkSimpleEsp8266.h"
#include "DallasTemperature.h"
#include "OneWire.h"
#include "WiFiClient.h"
#include "UrlEncode.h"

/*
* Mendeklarasikan PIN
*Pin menggunakan GPIO
*/
int buzzer = 5;
int smoke_sensor = A0;
int flame_sensor = 13;
int pump = 4;
int temp_sensor = 0;
int smoke_sensor_read;
int flame_sensor_read;
String Temp;

/*
* definisikan ssid dan password WiFi sesuai yang digunakan
* definisikan nomor WA dan apiKey yang terdaftar pada WhatsApp Bot
*/
const char* ssid = "Galaxy";
const char* password = "qhng4xxx";
const String phoneNumber = "+62858481xxxxx";
const String apiKey= "1405xxx";

OneWire oneWire(temp_sensor);
DallasTemperature sensors(&oneWire);

//membuat fungsi untuk mengirim pesan ke WhatsApp Bot
void sendMessage(String message) {
  String url = "http://api.callmebot.com/whatsapp.php?phone=" + phoneNumber + "&apikey=" + apiKey + "&text=" + urlEncode(message);
  WiFiClient client;
  HTTPClient http;
  http.begin(client, url);

  http.addHeader("Content-Type", "application/x-www-form-urlencoded");

  int httpResponseCode = http.POST(url);
  if (httpResponseCode == 200) {
    Serial.print("Alert Sent Succesfully");
  } else {
    Serial.println("Error sending the message");
    Serial.print("HTTP response code: ");
    Serial.println("httpResponseCode");
  }

  http.end();
  
}

void setup() {
  // put your setup code here, to run once:
  pinMode(buzzer, OUTPUT);
  pinMode(pump, OUTPUT);
  pinMode(smoke_sensor, INPUT);
  pinMode(flame_sensor, INPUT);

  digitalWrite(buzzer, LOW);
  digitalWrite(pump, LOW);
  sensors.begin();

  Serial.begin(115200);
  Serial.println("System Loading...");
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi");
  Serial.println("Connecting");

  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  // put your main code here, to run repeatedly:
  smoke_sensor_read = analogRead(smoke_sensor);
  Serial.print("Smoke Indicator: ");
  Serial.println(smoke_sensor_read);

  flame_sensor_read = digitalRead(flame_sensor);
  Serial.print("flame Indicator: ");
  Serial.println(flame_sensor_read);

  sensors.requestTemperatures();
  Temp = sensors.getTempCByIndex(0);
  Serial.print("Temperature is: ");
  Serial.println(Temp);
  
  if ((smoke_sensor_read > 400) or (flame_sensor_read == 0)) {
    digitalWrite(buzzer, HIGH);
    delay(100);
    digitalWrite(buzzer, LOW);
    delay(100);
    digitalWrite(pump, LOW);
    sendMessage("ALERT! FIRE DETECTED!!!");
    sendMessage("Temperature: " + String(Temp));
  } else {
    digitalWrite(pump, HIGH);
    digitalWrite(buzzer, LOW);
  } delay(1000);
}
