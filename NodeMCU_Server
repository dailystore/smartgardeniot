#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

const char* ssid = "iPhone";
const char* password = "XXXXXXXX";

ESP8266WebServer server(80);

const int led = 2;

/* this array contains the web will be responded to client
it uses jquery for making GET request and processing slider UI control */
char res[900]=
"<!DOCTYPE html>\
<html>\
<head>\
<meta charset='utf-8'>\
<H1>SAPVN-IOT.com Blinking Led</H1>\
<link href='https://code.jquery.com/ui/1.10.4/themes/ui-lightness/jquery-ui.css' rel='stylesheet'>\
<script src='https://code.jquery.com/jquery-1.10.2.js'></script>\
<script src='https://code.jquery.com/ui/1.10.4/jquery-ui.js'></script>\
<script>\
$(function() {\
$('#sliVal').html('Angle: 0');\
$('#slider').slider({\
    orientation:'vertical',value:0,min: 0,max: 10,step: 1\
});\
$('#slider').slider().bind({\
slidestop: function(e,ui){\
    $('#res').css('background','red');\
    $('#sliVal').html('Angle: '+ui.value);\
    $.get('/ang?val=' + ui.value, function(d, s){\
        $('#res').css('background','green');\
        $('#res').html(s);\
    }); \
}\
});\
});\
</script>\
</head>\
<body>\
<div id='slider'></div></br>\
<div id='sliVal'></div>\
<div id='res'></div>\
</body>\
</html>";

void handleRoot() {
  digitalWrite(led, 1);
//  server.send(200, "text/plain", "hello from esp8266!");
  server.send(200, "text/html", res);
  digitalWrite(led, 0);
}

void handleNotFound(){
  digitalWrite(led, 1);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  digitalWrite(led, 0);
}

void servoGo(int newAngle)
{
  for (int x=0; x<newAngle; x++) {
    digitalWrite(led, HIGH);
    delay(300);
    digitalWrite(led, LOW);
    delay(300);
  }

}

/* this callback will be invoked when get servo rotation request */
void handleServo() {
  //Serial.println(server.argName(0));
  int newAngle = server.arg(0).toInt();
  servoGo(newAngle);
  server.send(200, "text/html", "ok");
}

void setup(void){
  pinMode(led, OUTPUT);
  digitalWrite(led, 0);
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);
  server.on("/ang", handleServo);

  server.on("/inline", [](){
    server.send(200, "text/plain", "this works as well");
  });

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void){
  server.handleClient();
}
