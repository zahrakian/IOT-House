#include <ESP8266WiFi.h>
#include <espnow.h>
#include <Wire.h>
#include <SoftwareSerial.h>
#include <string>
#include <DHT.h>
using namespace std;


// RX, TX pins for software serial
SoftwareSerial mySerial(D2, D3);  
// Set DHT pin:
#define DHTPIN D4
// Set DHT type
#define DHTTYPE DHT11   // DHT 11 
// Initialize DHT sensor for normal 16mhz 
DHT dht = DHT(DHTPIN, DHTTYPE);
int doorstatesend;
int t;
int h;
int Light;
int movement;

typedef struct struct_message {
    int id;
    int doorstate ; 
} struct_message;

struct_message myData;
struct_message board1;
struct_message boardsStruct[1] = {board1};

void OnDataRecv(uint8_t * mac_addr, uint8_t *incomingData, uint8_t len) {
  char macStr[18];
  Serial.print("Packet received from: ");
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.println(macStr);
  memcpy(&myData, incomingData, sizeof(myData));
  Serial.printf("Board ID %u: %u bytes\n", myData.id, len);
  // Update the structures with the new incoming data
  boardsStruct[myData.id-1].doorstate = myData.doorstate;
  Serial.printf("x value: %d \n", boardsStruct[myData.id-1].doorstate);
  Serial.println();
}
 
void setup() {
  Serial.begin(115200);
  mySerial.begin(9600);      // Initialize the software serial port
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  pinMode(A0, INPUT);
  pinMode(D5, INPUT);
  dht.begin();
    delay(1000);


  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
  esp_now_register_recv_cb(OnDataRecv);
}

void loop(){
  delay(1000);

int board1door = boardsStruct[0].doorstate;
doorstatesend=board1door;
//mySerial.println(doorstatesend);  // Send a message via software serial
//mySerial.print(gas);  // Send a message via software serial
//Serial.println(gas);  // Send a message via software serial

if (board1door == HIGH) { // if the door is open
   Serial.println("Door is open!");
  } else { // if the door is closed
    Serial.println("Door is closed.");
  }
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  // Read the humidity in %:
   h = dht.readHumidity();
  // Read the temperature as Celsius:
   t = dht.readTemperature();
  // Read the temperature as Fahrenheit:
 // float f = dht.readTemperature(true);
  // Check if any reads failed and exit early (to try again):
  //if (isnan(h) || isnan(t) || isnan(f)) {
   // Serial.println("Failed to read from DHT sensor!");
   // return;
 // }
Light =analogRead(A0);
movement= digitalRead(D5);
  Serial.print("door ");
  Serial.println(doorstatesend);
  Serial.print("h:");
  Serial.println(h);
  Serial.print("t:");
  Serial.println(t);
  Serial.print("light :");
  Serial.println(Light);
  Serial.print("movement :");
  Serial.println(movement);
  Serial.println("-----------------------------------");
  mySerial.println(doorstatesend);
  mySerial.println(h);
  mySerial.println(t);
  mySerial.println(Light);
  mySerial.println(movement); 
}
