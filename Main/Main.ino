#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <SFE_BMP180.h>
#include <Wire.h>
#include <BlynkSimpleEsp8266.h>
#include "index.h" 
#include "DHTesp.h" 

#define LED 2 //On board LED
#define DHTpin 14 //D5 of NodeMCU is GPIO14

SFE_BMP180 pressure;

DHTesp dht;

char auth[] = "_-M9dujofBAS6MLnz-W1X5fr5LXOsIJW";
const char* ssid = "Zahrann";
const char* password = "123456789";

BlynkTimer timer;
ESP8266WebServer server(80);

void handleRoot() {
  String s = MAIN_page; //Read HTML contents
  server.send(200, "text/html", s);
}

float humidity, temperature;
int z, k ; double pp;
char status;
double T, P, p0, a;
double Tdeg, Tfar, phg, pmb;

void handleADC() {
  char status;
  double T, P;
  double pmb;

  status = pressure.getPressure(P, T);
  pmb = P;

  int rain = analogRead(A0);
  int value = map(rain, 0, 1023, 100, 0);
  //Create JSON data
  String data = "{\"Rain\":\"" + String(value) + "\",\"Pressuremb\":\"" + String(pmb) + "\",\"Pressurehg\":\"" + String(phg) + "\", \"Temperature\":\"" + String(temperature) + "\", \"Humidity\":\"" + String(humidity) + "\"}";

  digitalWrite(LED, !digitalRead(LED)); //Toggle LED on data request ajax
  server.send(200, "text/plane", data); //Send ADC value, temperature and humidity JSON to client ajax request

  delay(dht.getMinimumSamplingPeriod());

  humidity = dht.getHumidity();
  temperature = dht.getTemperature();

  BlynkTimer timer;
  Serial.print("H:");
  Serial.println(humidity);
  Serial.print("T:");
  Serial.println(temperature);
  Serial.print("R:");
  Serial.println(value);
  Serial.print("P:");
  Serial.println(P);
  z = value;
  k = temperature;
  pp=pmb;
}
void myTimerEvent(){
Blynk.virtualWrite(V5, temperature);
Blynk.virtualWrite(V6, humidity);
Blynk.virtualWrite(V7, z);
Blynk.virtualWrite(V8, pp);
if (z > 50)
{
  Blynk.email("mohamedzaahran1@gmail.com", "Weather  Alert", "It is Rainy!");
  Blynk.notify("Weather  Alert - It is Rainy!");
}
if (k > 30)
{

  Blynk.email("mohamedzaahran1@gmail.com", "Weather  Alert", "It is too hot!");
  Blynk.notify("Weather  Alert - It is too hot");
}
if (k < 22 && k >0)
{
  Blynk.email("mohamedzaahran1@gmail.com", "Weather  Alert", "It is cold!");
  Blynk.notify("Weather  Alert - It is cold");
}


}

void setup()
{
  Serial.begin(115200);
  Serial.println();

  // dht11 Sensor

  dht.setup(DHTpin, DHTesp::DHT11); //for DHT11 Connect DHT sensor to GPIO 17
  pinMode(LED, OUTPUT);
  Blynk.begin(auth, ssid, password);

  //BMP180 Sensor

  WiFi.begin(ssid, password);
  Serial.println("");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP()); 

    server.on("/", handleRoot); //Which routine to handle at root location. This is display page
  server.on("/readADC", handleADC); //This page is called by java Script AJAX
  timer.setInterval(1000L, myTimerEvent);

  server.begin();
  Serial.println("our Website started");


}

void loop()
{
  server.handleClient();//Handle client requests
  Blynk.run();
  timer.run();
}