#include <ESP8266WiFi.h>
#include <espnow.h>

uint8_t broadcastAddress[] = {0xA0, 0x20, 0xA6, 0x10, 0x44, 0x42};

#define BOARD_ID 1

typedef struct struct_message {
    int id;
    int doorstate;
} struct_message;

struct_message myData;

unsigned long lastTime = 0;
unsigned long timerDelay = 10000;

void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
  Serial.print("\r\nLast Packet Send Status: ");
  if (sendStatus == 0){
    Serial.println("Delivery success");
  }
  else{
    Serial.println("Delivery fail");
  }
}

const int sensorPin = D4; // the pin that the sensor is attached to



 
void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  } 
   if (esp_now_init() == 0) {
    Serial.println("SUCCESS initializing ESP-NOW");
    return;
  } 
  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
  esp_now_register_send_cb(OnDataSent);
  esp_now_add_peer(broadcastAddress, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);
  pinMode(sensorPin, INPUT);  // set the sensor pin as an input


}
 
void loop() {
 int sensorValue = digitalRead(sensorPin);  // read the sensor value
 Serial.println(sensorValue);
 Serial.println("----------------------------------");


 
  if ((millis() - lastTime) > timerDelay) {
    myData.id = BOARD_ID;
    myData.doorstate = sensorValue;
    esp_now_send(0, (uint8_t *) &myData, sizeof(myData));
    lastTime = millis();
  }
}
