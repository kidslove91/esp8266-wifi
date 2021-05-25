#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <BlynkSimpleEsp8266.h>
#include <ESP8266mDNS.h>  //Thư viện mDNS
#include <WebSocketsServer.h>
WebSocketsServer webSocket = WebSocketsServer(81);
ESP8266WebServer webServer(80);
BlynkTimer timer_update;
char* ssid = "E-SMART";
char* pass = "12345678";
const int BTN = 0;
//============Biến chứa mã HTLM Website==========================================//
const char MainPage[] PROGMEM = R"=====(
  <!DOCTYPE html> 
  <html>
   <head> 
       <title>HOME PAGE</title> 
       <style> 
          body {text-align:center;}
          h1 {color:#003399;}
          a {text-decoration: none;color:#FFFFFF;}
          .bt_on {
            height:50px; 
            width:100px; 
            margin:10px 0;
            background-color:#FF6600;
            border-radius:5px;
          }
          .bt_off {
            height:50px; 
            width:100px; 
            margin:10px 0;
            background-color:#00FF00;
            border-radius:5px;
          }
       </style>
       <meta name="viewport" content="width=device-width,user-scalable=0" charset="UTF-8">
   </head>
   <body onload="javascript:init()"> 
      <h1>QUANG HUY. SDT 0968973860</h1> 
      <div>Trạng thái trên chân D1: <b><pan id="trangthaiD1"><pan></b></div>
      <div>
        <button class="bt_on" onclick="sendData('onD1')">ON</button>
        <button class="bt_off" onclick="sendData('offD1')">OFF</button>
      </div>
      <div>Trạng thái trên chân D2: <b><pan id="trangthaiD2"><pan></b></div>
      <div>
        <button class="bt_on" onclick="sendData('onD2')">ON</button>
        <button class="bt_off" onclick="sendData('offD2')">OFF</button>
      </div>
      <script>
            var Socket;      //Khai báo biến Socket
            function init(){
              //Khởi tạo websocket
              Socket = new WebSocket('ws://' + window.location.hostname + ':81/');
              //Nhận broadcase từ server
              Socket.onmessage = function(event){   
                JSONobj = JSON.parse(event.data);   //Tách dữ liệu json
                document.getElementById("trangthaiD1").innerHTML = JSONobj.D1;
                document.getElementById("trangthaiD2").innerHTML = JSONobj.D2;
              }
            }
            //-----------Gửi dữ liệu lên Server-------------------
            function sendData(data){
              Socket.send(data);
            }
      </script>
   </body> 
  </html>
)=====";
//========================================================//
void setup() {
  //===============Thiết lập chân IO======================//
  pinMode(D1,OUTPUT);
  pinMode(D2,OUTPUT);
   pinMode(BTN, INPUT);
  digitalWrite(D1,HIGH);
  digitalWrite(D2,HIGH);
  //===============Thiết lập kết nối WiFi=================//
  //WiFi.begin(ssid,pass);
  Serial.begin(9600);
  //Serial.print("Connecting");
//  while(WiFi.status()!=WL_CONNECTED){
//    delay(500);
//    Serial.print("...");
//  }
//  Serial.println(WiFi.localIP());
  WiFi.disconnect();
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, pass);
  //======Khởi tạo mDNS cấp phát Tên miền E-SMART HOME====//
  if (!MDNS.begin("E-SMARTHOME")) {
    Serial.println("Error setting up MDNS responder!");
    while (1) {
      delay(1000);
    }
  }
  Serial.println("mDNS responder started");
  MDNS.addService("http","tcp",80);
  //=================Khởi tạo Web Server=================//    
  webServerStart();
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
  timer_update.setInterval(1000L, updateStateD);
}
void loop() {
  MDNS.update();  //mDNS
  webServer.handleClient();
  webSocket.loop();
  timer_update.run();
  static bool isPressed = false;
  if (!isPressed && digitalRead(BTN) == 0) { //Nhấn nút nhấn GPIO0
    isPressed = true;
    webSocket.broadcastTXT("onD1");
  } else if (isPressed && digitalRead(BTN)) { //Nhả nút nhấn GPIO0
    isPressed = false;
    webSocket.broadcastTXT("offD1");
  }
}
//===================Chương trình con====================//
void webServerStart(){
  webServer.on("/",mainpage);
  webServer.begin();
}
void mainpage(){
  String s = FPSTR(MainPage);
  webServer.send(200,"text/html",s);
}
void webSocketEvent(uint8_t num, WStype_t type,
                    uint8_t * payload,
                    size_t length)
{ if(type==WStype_TEXT)
{
  String payloadString = (const char *)payload;
  Serial.print("payloadString= ");
  Serial.println(payloadString);
  Serial.println("number: ");
  Serial.println(*(&num));
  Serial.println(*(&num+1));
  Serial.println(*(&num+2));
  Serial.println(*(&num+3));
  String s = "";
  if(payloadString == "onD1"){
    digitalWrite(D1,LOW);
  }else if(payloadString == "offD1"){
    digitalWrite(D1,HIGH);
  }
  if(payloadString == "onD2"){
    digitalWrite(D2,LOW);
  }else if(payloadString == "offD2"){
    digitalWrite(D2,HIGH);
  }
}
if(type==WStype_BIN)
{
  Serial.println("so nhan duoc:");
  for(int i=0;i<length;i++)
  {
    Serial.print(payload[i]);
    Serial.print("/  ");
  }
}
}
ICACHE_RAM_ATTR void updateStateD(){
  String d1 = "";
  String d2 = "";
  if(digitalRead(D1) == LOW){
    d1 = "ON";
  }else{
    d1 = "OFF"; 
  }
  if(digitalRead(D2) == LOW){
    d2 = "ON";
  }else{
    d2 = "OFF"; 
  }
  String JSONtxt = "{\"D1\":\""+d1+"\",\"D2\":\""+d2+"\"}";
  webSocket.broadcastTXT(JSONtxt);
  Serial.println(JSONtxt);
}
