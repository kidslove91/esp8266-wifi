#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WebSocketsClient.h> //https://github.com/Links2004/arduinoWebSockets
WebSocketsClient webSocket;
const char* ssid = "E-SMART"; //Đổi thành wifi của bạn
const char* password = "12345678"; //Đổi pass luôn
const char* ip_host = "192.168.4.1"; //Đổi luôn IP host của PC nha
const uint16_t port = 81; //Port thích đổi thì phải đổi ở server nữa
const int LED = 2;
const int BTN = 0;
void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
  String payloadString = (const char *)payload;
  Serial.print("payloadString= ");
  Serial.println(payloadString);
  String s = "";
  if(payloadString == "onD1"){
    digitalWrite(LED,LOW);
  }else if(payloadString == "offD1"){
    digitalWrite(LED,HIGH);
}
}
void setup() {
  pinMode(LED, OUTPUT);
  pinMode(BTN, INPUT);
  Serial.begin(9600);
  Serial.println("ESP8266 Websocket Client");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  webSocket.begin(ip_host, port);
  webSocket.onEvent(webSocketEvent);
}
void loop() {
  webSocket.loop();
  static bool isPressed = false;
  if (!isPressed && digitalRead(BTN) == 0) { //Nhấn nút nhấn GPIO0
    isPressed = true;
    uint8_t va[1]={0};
    va[0]=1;
    webSocket.sendBIN(va,2);
  } else if (isPressed && digitalRead(BTN)) { //Nhả nút nhấn GPIO0
    isPressed = false;
    webSocket.sendTXT("offD1");
  }
}
