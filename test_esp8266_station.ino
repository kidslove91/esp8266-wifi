/* Cắm cable kết nối
 * Mở Device Manger để xem port
 * Vào Tools -> Board -> NodeMCU 1.0 (ESP-12E Module)
 * Vào Tools -> Port -> Chọn Port kết nối.
 * Nạp code xong dùng điện thoại có kết nối Wifi truy cập vào wifi có tên NSHOP, mật khẩu: 12345678
 */

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

const char *ssid = "NSHOP";
const char *password = "12345678";
int stateLED = LOW;

ESP8266WebServer server(80);

void handleRoot() {
  response();
}

void handleLedOn() {
  stateLED = HIGH;
  digitalWrite(4, stateLED);
  digitalWrite(5, stateLED);
  response();
}

void handleLedOff() {
  stateLED = LOW;
  digitalWrite(4, stateLED);
  digitalWrite(5, stateLED);
  response();
}
const String HtmlHtml = "<html><head>"                        "<BODY align=center>"                        "<meta name=viewport"" content=width=device-width, initial-scale=1 /></head>";
const String HtmlHtmlClose = "</html>";
const String HtmlTitle = "<h1>NamVietGroup</h1><br/>";
const String HtmlLedStateLow = "<big>LED is now <b>OFF</b></big><br/>";
const String HtmlLedStateHigh = "<big>LED is now <b>ON</b></big><br/>";
const String HtmlButtons =  "<a href=LEDOn""><button style=display: block; width: 100%;"">ON</button></a><br/>"  "<a href=LEDOff""><button style=display: block; width: 100%;"">OFF</button></a><br/>";

void response() {
  String htmlRes = HtmlHtml + HtmlTitle;
  if (stateLED == LOW) {
    htmlRes += HtmlLedStateLow;
  } else {
    htmlRes += HtmlLedStateHigh;
  }

  htmlRes += HtmlButtons;
  htmlRes += HtmlHtmlClose;

  server.send(200, "text/html", htmlRes);
}

void setup() {
  delay(1000);
  Serial.begin(115200);
  Serial.println();

  WiFi.softAP(ssid, password);

  IPAddress apip = WiFi.softAPIP();
  Serial.print("visit: ");
  Serial.println(apip);
  server.on("/", handleRoot);
  server.on("/LEDOn", handleLedOn);
  server.on("/LEDOff", handleLedOff);
  server.begin();
  Serial.println("HTTP server beginned");
  pinMode(4, OUTPUT);
  digitalWrite(0, stateLED);
  pinMode(5, OUTPUT);
  digitalWrite(1, stateLED);
}

void loop() {
  server.handleClient();
}
