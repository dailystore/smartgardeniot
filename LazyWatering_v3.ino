/*
  Lazy Watering ver 0.3:
  - Read sensor values
  - Connect MQTT broker + send sensor value
  - Message type for NodeMCU covered

*/

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <NTPtimeESP.h>
#include <DHT.h>

#define DEBUG_ON

ESP8266WebServer server(80);

// Sensors define
#define DHTPIN D4 // Chân kết nối với DHT11
#define DHTTYPE DHT11   // Loại cảm biến là DHT11
DHT dht(DHTPIN, DHTTYPE);
long Interval_Sensor = 10000;
float humi = 0;
float temp = 0;
long last_SensorData = 0;

// Watering define
int relayPin = 4; //Bom
int Water_Mode = 2;
long Water_Interval = 15000;
long Water_Duration = 5000;
long Water_ImmDuration = 10000;
long last_Water = 0;
long Water_Elapsed = 0;

// Epoch get time
NTPtime NTPch("ch.pool.ntp.org");   // Choose server pool as required

// Wifi and MQTT connection
const char* ssid = "sapvniot";
const char* password = "123456";
const char* mqtt_server = "m14.cloudmqtt.com";
const size_t bufferSize = 300;
char jsonChar[bufferSize];
WiFiClient espClient;
PubSubClient client(espClient);
int value = 0;
strDateTime dateTime;

/* this array contains the web will be responded to client
  it uses jquery for making GET request and processing slider UI control */
char res[900] =
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

void setup()
{
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  pinMode(relayPin, OUTPUT);     // Initialize the BUILTIN_LED pin as an output

  Serial.begin(115200);
  dht.begin();
  setup_wifi();
  client.setServer(mqtt_server, 11543);
  client.setCallback(callback);

  // Setup Raspberry connection
  if (MDNS.begin("Node1")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);
  server.on("/triggerWater", triggerWater);

  server.on("/inline", []() {
    server.send(200, "text/plain", "this works as well");
  });

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("connection with Raspberry started <3");
}

void setup_wifi()
{

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length)
{
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1')
  {
    digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is acive low on the ESP-01)
  } else
  {
    digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
  }

}

void reconnect()
{
  // Loop until we're reconnected
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client", "sapvniot", "Abcd01234"))
    {
      Serial.println("connected");
      // Once connected, publish an announcement...
      //      client.publish("iot/data/iotmmsp2000064899trial/v1/f6bcf80c-8180-43a5-9417-fc4018c6971d", "hello world");
      // ... and resubscribe
      client.subscribe("inTopic");
    }

    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void loop()
{
  // Connect to Raspberry
  server.handleClient();


  // Collect TimeStamp
  dateTime = NTPch.getNTPtime(7, 0);

  // check MQTT connection
  if (!client.connected())
  {
    reconnect();
  }
  client.loop();
  long now = millis();

  // Read and send sensor data every "Interval_Sensor" time
  if (now - last_SensorData > Interval_Sensor)
  {
    readSensors();
    last_SensorData = now;
    MsgMaker();
    client.publish("iot/data/iotmmsp2000064899trial/v1/f6bcf80c-8180-43a5-9417-fc4018c6971d", jsonChar);
  }

  if (Water_Mode == 0)
  {
    // If reach "Water_Interval" time
    if (now - last_Water > Water_Interval)
    {
      last_Water = now;
      Water_Elapsed = 0;
      //Serial.print("Watering for: ");
      //Serial.print(Water_Duration);
      //Serial.println("ms");
    }

    // Watering in a "Water_Duration" time
    if (Water_Elapsed < Water_Duration)
    {
      digitalWrite(relayPin, LOW);
      Water_Elapsed = now - last_Water;
    }
    else
    {
      digitalWrite(relayPin, HIGH);
    }
  }
  else if (Water_Mode == 1)
  {
    if (Water_Elapsed < Water_ImmDuration)
    {
      digitalWrite(relayPin, LOW);
      Water_Elapsed = now - last_Water;
    }
    else
    {
      digitalWrite(relayPin, HIGH);
      last_Water = now;
      Water_Elapsed = 0;
      Water_Mode = 2;
    }
  }
  else if (Water_Mode == 2)
  {
    digitalWrite(relayPin, HIGH);
  }
}

char MsgMaker()
{
  /* Wait for Long's part
    double sen_Temp = Get_Tenperature();
    double sen_Humi = Get_Humidity();
    double vol_Na = Get_vol_Na();
    double vol_Ka = Get_vol_Ka();
  */

  DynamicJsonBuffer jsonBuffer(bufferSize);

  JsonObject& JsOb = jsonBuffer.createObject();
  JsOb["mode"] = "sync";
  JsOb["messageType"] = "66264498cb1cc090fc4b";

  JsonArray& messages = JsOb.createNestedArray("messages");

  JsonObject& messages_0 = messages.createNestedObject();
  messages_0["timestamp"] = dateTime.epochTime;
  messages_0["sen_Temperature"] = temp;
  messages_0["sen_Humidity"] = humi;
  messages_0["vol_Fert_Na"] = 1200;
  messages_0["vol_Fert_Ka"] = 1500;
  messages_0["vol_Total"] = 0;


  JsOb.printTo((char*)jsonChar, JsOb.measureLength() + 1);
  Serial.print("Publish message: ");
  Serial.println(jsonChar);

}

void readSensors()
{
  // Đọc nhiệt độ độ ẩm từ DHT22
  humi = dht.readHumidity();
  temp = dht.readTemperature();
  if (isnan(humi) || isnan(temp))
  {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  else
  {
    Serial.print("Temperature: ");
    Serial.print(temp);
    Serial.print(" oC Humidity: ");
    Serial.print(humi);
    Serial.println("sending to MQTT");
  }
}

void handleRoot()
{
  Serial.println("Node-Rasp server started!");
  server.send(200, "text/html", res);
}

void handleNotFound()
{
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

/* this callback will be invoked when get servo rotation request */
void triggerWater() {
  
  Water_Mode = server.arg(0).toInt();
  Water_Interval = server.arg("interval").toInt();
  Water_Duration = server.arg("duration").toInt();

  Serial.print("Water mode: ");
  Serial.println(Water_Mode);
  Serial.print("Water interval: ");
  Serial.println(Water_Interval);
  Serial.print("Water duration: ");
  Serial.println(Water_Duration);
  
  // Define 2 water modes
  // Mode 0: Interval
  if (Water_Mode == 0)
  {
    Water_Interval = server.arg(1).toInt();
    Water_Duration = server.arg(2).toInt();
    if (Water_Interval == 0) Water_ImmDuration = 15000;
    if (Water_Duration == 0) Water_ImmDuration = 5000;
  }

  // Mode 1: Immediately
  else if (Water_Mode == 1)
  {
    Water_Elapsed = 0;
    last_Water = millis();
    Water_ImmDuration = server.arg(1).toInt();
    if (Water_ImmDuration == 0) Water_ImmDuration = 10000;
  }

  // Mode 2: Stop
  else if (Water_Mode == 2)
  {
    
  }

  server.send(200, "text/html", "ok");
}
