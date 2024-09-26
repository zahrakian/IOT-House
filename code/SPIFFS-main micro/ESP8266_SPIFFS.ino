#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <FS.h>
#include <SoftwareSerial.h>
#include <string>
using namespace std;


// RX, TX pins for software serial
SoftwareSerial mySerial(D2, D3);  

const char* ssid = "poca";
const char* password = "12345678";
//pin assignment
int relay_1 = D7;  //relay 1 pin to activate coil
int relay_2 = D8;  //relay 2 pin to activate coil

//Actuator door open and close temperature values in celsius
int open_door_temp = 27;
int close_door_temp = 24;


float gas;
int door ;
int t;
int h;
int Light;
int movement;
int actuator=0;


// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

String getTemperature() {
  return String(t);
}
String getHumidity() {
  return String(h);
}

String getLight() {
  return String(Light);
}
String getGas() {
  return String(gas);
}
String getMovement() {
  String movementstring;
  if( movement==1){
    movementstring="Movement detected";
    }
  else {
    movementstring=" No movement detected ";
    
    }
  return movementstring;

}
String getDoor() {
  String doorstring;
  if( door==1){
    doorstring="Door is open";
    }
   else{
    doorstring ="Door is closed " ;
   }
  return doorstring;

}

String getActuator() {
  String actuatorstring;
  if( actuator==1){
    actuatorstring="Extend Adtuator";
    }
   else{
    actuatorstring ="Retract Actuator" ;
   }
  return actuatorstring;

}

String processor(const String& var){
  Serial.println(var);
  if (var == "TEMPERATURE"){
    return getTemperature();
  }
  else if (var == "HUMIDITY"){
    return getHumidity();
  }
  else if (var == "LIGHT"){
    return getLight();
  }
  else if (var == "DOOR"){
    return getDoor();
  }
  else if (var == "GAS"){
    return getGas();
  }
  else if (var == "MOVEMENT"){
    return getMovement();
  }
  else if (var == "ACTUATOR"){
    return getActuator();
    
  }
  
  return String();
}
 
void setup(){
  // Serial port for debugging purposes
  Serial.begin(115200);        // Initialize the hardware serial port
  mySerial.begin(9600);      // Initialize the software serial port  // Initialize SPIFFS
  if(!SPIFFS.begin()){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
 pinMode(A0, INPUT);
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  // Print ESP8266 Local IP Address
  Serial.println(WiFi.localIP());

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });
  server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", getTemperature().c_str());
  });
  server.on("/humidity", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", getHumidity().c_str());
  });
  server.on("/light", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", getLight().c_str());
  });
server.on("/gas", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", getGas().c_str());
  });
server.on("/door", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", getDoor().c_str());
  });
 server.on("/movement", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", getMovement().c_str());
  });
  // Start server
  server.on("/atuator", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", getActuator().c_str());
  });
  server.begin();

  pinMode(relay_1, OUTPUT); //defining relay 1 pin as output
  pinMode(relay_2, OUTPUT); //defining relay 2 pin as output
  
  digitalWrite(relay_1, LOW); //deactivating relay 1
  digitalWrite(relay_2, LOW); //deactivating relay 2
}
 
void loop(){
  // Wait a few seconds between measurements:
  delay(1000);
 gas=analogRead(A0);

  if (mySerial.available()) {
//gas = mySerial.read();
 door = mySerial.parseInt(); // Integer
 h = mySerial.parseInt();
 t= mySerial.parseInt();
 Light= mySerial.parseInt();
 movement = mySerial.parseInt();
  }
  Serial.print("door ");
  Serial.println(door);
    Serial.print("gas :");
  Serial.println(gas);
    Serial.print("h:");
 Serial.println(h);
  Serial.print("t:");
  Serial.println(t);
  Serial.print("light :");
  Serial.println(Light);
  Serial.print("move; ");
  Serial.println(movement);
  Serial.println("-----------------------------------");
   
  if (t > open_door_temp) {         //if the temperature value is higher than 27C, extend actuator
  
    digitalWrite(relay_1, HIGH);
    digitalWrite(relay_2, LOW);
    actuator = 1;
  
  }

  else if (t < close_door_temp) {     //if the temperature value is lower than 24C, retract actuator
    digitalWrite(relay_1, LOW);
    digitalWrite(relay_2, HIGH);
    actuator = 0;

  }

}
