#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WiFiMulti.h> 
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>
#include <Adafruit_NeoPixel.h>
 
#define D1  5
#define D2  4
#define D3  0
#define D4  2
#define N_LEDS 10

ESP8266WiFiMulti wifiMulti;     // Create an instance of the ESP8266WiFiMulti class, called 'wifiMulti'

ESP8266WebServer server(80);    // Create a webserver object that listens for HTTP request on port 80

Adafruit_NeoPixel strip = Adafruit_NeoPixel(N_LEDS, D1, NEO_RGB + NEO_KHZ400);

const size_t STRIPS = 4;

Adafruit_NeoPixel strips[STRIPS] = {
  Adafruit_NeoPixel(N_LEDS, D1, NEO_RGB + NEO_KHZ400),
  Adafruit_NeoPixel(N_LEDS, D2, NEO_RGB + NEO_KHZ400),
  Adafruit_NeoPixel(N_LEDS, D3, NEO_RGB + NEO_KHZ400),
  Adafruit_NeoPixel(N_LEDS, D4, NEO_RGB + NEO_KHZ400)
};

void setup(void){
  Serial.begin(115200);         // Start the Serial communication to send messages to the computer
  delay(10);
  Serial.println('\n');

  wifiMulti.addAP("SID", "PASSWORD");   // add Wi-Fi networks you want to connect to

  Serial.println("Connecting ...");
  int i = 0;
  while (wifiMulti.run() != WL_CONNECTED) { // Wait for the Wi-Fi to connect: scan for Wi-Fi networks, and connect to the strongest of the networks above
    delay(250);
    Serial.print('.');
  }
  Serial.println('\n');
  Serial.print("Connected to ");
  Serial.println(WiFi.SSID());               // Tell us what network we're connected to
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());            // Send the IP address of the ESP8266 to the computer

  if (MDNS.begin("esp8266")) {              // Start the mDNS responder for esp8266.local
    Serial.println("mDNS responder started");
  } else {
    Serial.println("Error setting up MDNS responder!");
  }

  server.on("/", HTTP_GET, handleRoot);
  server.on("/", HTTP_POST, handlePost);
  server.on("/show", HTTP_POST, stripsShow);
  
  server.begin();                            // Actually start the server
  Serial.println("HTTP server started");
  stripsBegin();
  stripsClear();
}

void stripsBegin(){
  for(uint16_t i=0; i<STRIPS; i++) {
    Serial.println("Starting strip " + String(i));
    strips[i].begin();
  }  
}


void stripsClear(){
  for(uint16_t i=0; i<STRIPS; i++) {
    Serial.println("Clearing strip " + String(i));
    for(uint16_t j=0; j<strips[i].numPixels(); j++) {
      strips[i].setPixelColor(j, 0);
      Serial.println("Clearing pixel string:" + String(i) + " LED:" + String(j));
    }
  }
  stripsShow();
}

void loop(void){
  server.handleClient();                     // Listen for HTTP requests from clients
}

void handleRoot() {                          // When URI / is requested, send a web page with a button to toggle the LED
  server.send(200, "application/json");
}

void handlePost() {                          // When URI / is requested, send a web page with a button to toggle the LED
  int strip = server.arg("strip").toInt();
  int led = server.arg("led").toInt();
  int red =  server.arg("red").toInt();
  int green = server.arg("green").toInt();
  int blue = server.arg("blue").toInt();


  if(strip < 0 || strip > STRIPS-1) {
    server.send(400,"text/plain", "Strip ID does not exist");
  }

  
  strips[strip].setPixelColor(
    led,
    strips[strip].Color(
      red,
      green,
      blue
    )
  );
      
  server.send(
    200,
    "text/plain",
    "Strip: " + String(strip) + "\n" + 
    "LED: " + String(led) + "\n" + 
    "R: " + String(red) + "\n" + 
    "G: " + String(green) + "\n" + 
    "B: " + String(blue)
  );
}

void stripsShow()
{
  for(uint16_t i=0; i<STRIPS; i++) {
    Serial.println("Showing strip: " + String(i));
    strips[i].show();
  }
  server.send(204, "application/json");
}
