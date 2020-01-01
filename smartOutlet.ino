#include <ESP8266WiFi.h>
#include <PubSubClient.h>

char randomNumber[5000];

const char* ssid = "twinky-tpl"; // Enter your WiFi name
const char* password =  "J.e(hA9xQ7uj"; // Enter WiFi password
const char* mqttServer = "4ln2jdv1ik-mqtt.ddns.net";
const int mqttPort = 1883;
const char* mqttUser = "";
const char* mqttPassword = "";
String payloadValue;
 
WiFiClient espClient;



//MQTT Object
PubSubClient client(espClient);

void wifiConnect() {

 Serial.println();
 Serial.print("Connecting to network: ");
 Serial.println(ssid);
 Serial.print("MAC: ");
 Serial.println(WiFi.macAddress());
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("Local IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();
}

void subscribeQueues() {
  client.subscribe("relays/relay1");
}

void mqttReconnect() {
    Serial.println("Reconnecting to MQTT...");
 
    if (client.connect("ESP8266Client", mqttUser, mqttPassword )) {
 
      Serial.println("connected");  
      subscribeQueues();
    } else {
 
      Serial.print("failed with state ");
      Serial.println(client.state());
      delay(2000);
 
    }
}

void setup() {

  pinMode(13,OUTPUT);
  Serial.begin(115200);
 
   WiFi.begin(ssid, password);
   wifiConnect();
  
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
 
  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
 
    if (client.connect("ESP8266Client", mqttUser, mqttPassword )) {
 
      Serial.println("connected");  
 
    } else {
 
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);
 
    }
  }

  client.publish("relays/status","Online");
  client.publish("relays/relay1_init", "false"); //Topic name
  subscribeQueues();
}


void callback(char* topic, byte* payload, unsigned int length) {

  
  payloadValue="";
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
 
  Serial.print("Message:");
  for (int i = 0; i < length; i++) {
    //Serial.print(payload[i]);
    //Serial.print("-");
    payloadValue+=(char)payload[i];
    /*switch (atoi(payloadValue)) {
      case 0:
        digitalWrite(13,LOW);
        break;
      case 1:
        digitalWrite(13,HIGH);
        break;
    }*/
  }

  itoa(random(5000),randomNumber,10);
  if(payloadValue=="false") {
    Serial.println("Relay turned off");
    digitalWrite(13,LOW);
    client.publish("relays/heartbeat", randomNumber); //Topic name
  } else if(payloadValue=="true") {
    Serial.println("Relay turned on");
    digitalWrite(13,HIGH);
    client.publish("relays/hearbeat", randomNumber); //Topic name
  } else {
    Serial.print("Payload: ");
    Serial.println(payloadValue);
  }
  //Serial.println((char*)random(5000));

  Serial.println("-----------------------");
 
}
 
void loop() {
  if(!client.connected()) {
    mqttReconnect();
  }

  if (WiFi.status() != WL_CONNECTED) {
     wifiConnect();
  }

  if(!client.loop()) {
    client.connect("ESP8266Client", mqttUser, mqttPassword );
    subscribeQueues();
  }
  //Serial.println("Another loop");
}
