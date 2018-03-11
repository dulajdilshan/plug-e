#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "ACS712.h"

const char* ssid = "404";
const char* password = "1matiop2";
const char* mqtt_server = "broker.mqttdashboard.com";
const char* outTopic = "outTopic95111";
const char* inTopic = "inTopic95111";

ACS712 sensor(ACS712_20A, A0);

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
char st[50];
int value = 0;

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  String cmd ="";
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    cmd = cmd + (char)payload[i];
  }
  Serial.println();

  //code
  if (cmd == "00") {
    digitalWrite(BUILTIN_LED, LOW);
  } else if(cmd == "11") {
    digitalWrite(BUILTIN_LED, HIGH);
  }else if(cmd == "01"){
    Serial.println("Send status");
  }else if(cmd == "10"){
    Serial.println("no cmd");
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP8266Client-";
    clientId += "asd001";
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      client.publish(outTopic, " Server started");
      client.subscribe(inTopic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void turnOff(){
}
void setup() {
  pinMode(BUILTIN_LED, OUTPUT);
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  sensor.calibrate();
}



void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  float U = 230;
  float I = sensor.getCurrentAC();
  float P = U * I;
  int state = 0;
//  Serial.println(I);
  delay(1000);
  
  long now = millis();
  if (now - lastMsg > 2000) {
    lastMsg = now;
    ++value;
    if(I>0.19){
      state = 1;
    }else{
      state = 0;
    }
    snprintf (msg, 75, "[outTopic] Current Val = %.2f", I);
    snprintf (st,75,"State: %1d", state);
//    Serial.print(msg);
    Serial.println(I);
//    client.publish(outTopic,msg);
    delay(1000);
//    client.publish(outTopic,st);
  }
}
