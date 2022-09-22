//#include <ESP8266WiFi.h>
//#include <PubSubClient.h>

#define PUSH_START_LED_PIN 3
#define HORN_LIGHTS_PIN 5
#define DOOR_UNLOCK_PIN 9
#define DOOR_LOCK_PIN 10
//#define IGNITION_PIN 12 
#define STARTER_PIN 13
#define ACC_IGNITION_PIN 14
#define PUSH_START_PIN 15

int ACC_STATE = 0;
int buttonNew;
int buttonOld = 1;
int buttonCheckDelay = 100;
int displace = 500;
int ENGINE_STATE = 0;
const int hornDelay = 30;
const long interval = 700;
int periode = 4000;
unsigned long previousMillis = 0;        // will store last time LED was updated
int PUSH_START_LED_STATE = 0;
unsigned long timeFade=0;
int value, value2 ;                           // light connected to digital pin 11

// WiFi
//const char *ssid = "SSID"; // Enter your WiFi name
//const char *password = "PASS";  // Enter WiFi password

// MQTT Broker
//const char *mqtt_broker = "192.168.1.88"; // Enter your WiFi or Ethernet IP
//const char *topic = "RemoteStart/RemoteStart";
//const int mqtt_port = 1883;

//WiFiClient espClient;
//PubSubClient client(espClient);

void setup() {
 Serial.begin(115200);
 pinMode(DOOR_LOCK_PIN, OUTPUT);
 digitalWrite(DOOR_LOCK_PIN, LOW);
 pinMode(DOOR_UNLOCK_PIN, OUTPUT);
 digitalWrite(DOOR_UNLOCK_PIN, LOW);
 pinMode(ACC_IGNITION_PIN, OUTPUT);
 digitalWrite(ACC_IGNITION_PIN, 1);
 pinMode(STARTER_PIN, OUTPUT);
 digitalWrite(STARTER_PIN, 1);
 pinMode(HORN_LIGHTS_PIN, OUTPUT);
 digitalWrite(HORN_LIGHTS_PIN, 1);
 pinMode(PUSH_START_PIN, INPUT);
 pinMode(PUSH_START_LED_PIN, OUTPUT);
 // connecting to a WiFi network
 /*
 WiFi.begin(ssid, password);
 while (WiFi.status() != WL_CONNECTED) 
 {
   delay(500);
   Serial.println("Connecting to WiFi..");
 }
 Serial.println("Connected to the WiFi network");
 //connecting to a mqtt broker
 client.setServer(mqtt_broker, mqtt_port);
 client.setCallback(callback);
 while (!client.connected()) 
 {
   String client_id = "rav4";
   Serial.printf("The client %s connects to mosquitto mqtt broker\n", client_id.c_str());
   if (client.connect(client_id.c_str())) 
   {
     Serial.println("Public emqx mqtt broker connected");
   } 
   else 
   {
    Serial.print("failed with state ");
    Serial.print(client.state());
    delay(2000);
   }
 }
// publish and subscribe
client.subscribe(topic);
*/
}

/*void callback(char *topic, byte *payload, unsigned int length) {
 Serial.print("Message arrived in topic: ");
 Serial.println(topic);
 Serial.print("Message:");
 String command = "";
 for (int i = 0; i < length; i++) {
  Serial.print((char) payload[i]);
   command += String((char) payload[i]);
 }
 Serial.println(command);
 if (command == "start")
 {
  Serial.println("Starting Engine");
  startingAdvisory();
  remoteStartEngine();
 }
 if (command == "stop")
 {
  Serial.println("Shutting Down Engine");
  stopEngine();
 }
 if (command == "lock")
 {
  Serial.println("locking doors");
  lockDoors();
 }
 if (command == "unlock")
 {
  Serial.println("unlocking doors");
  unlockDoors();
 }
 
 Serial.println();
 Serial.println(" - - - - - - - - - - - -");
}
*/

void loop() {
 if (ENGINE_STATE==0 && ACC_STATE==0)
 {
    fadePushButtonLed();
 } 
 if (ENGINE_STATE==0 && ACC_STATE==1)
 {
    flashPushButtonLed();
 }
 if (ENGINE_STATE==1 && ACC_STATE==1)
 {
    digitalWrite(PUSH_START_LED_PIN, 1);
 }
  
 buttonNew=digitalRead(PUSH_START_PIN);
  
 if(buttonOld==0 && buttonNew==1)
 {
    if (ENGINE_STATE==0 && ACC_STATE==0)
    {
      accessoryOn();
      Serial.println("ACC ON");
    }
    else if (ENGINE_STATE==0 && ACC_STATE==1)
    {
      startEngine();
      Serial.println("ENGINE ON");
    }
    else if (ENGINE_STATE==1 && ACC_STATE==1)
    {
      stopEngine();
    }
 }
  
 buttonOld=buttonNew;
 delay(buttonCheckDelay);
 //client.loop();
}

void remoteStartEngine(){
  ACC_STATE=1;
  ENGINE_STATE=1;
  digitalWrite(ACC_IGNITION_PIN, 0);
  delay(500);
  digitalWrite(STARTER_PIN, 0);
  delay(2000);
  digitalWrite(STARTER_PIN, 1);
  //client.publish("RemoteStart/Response", "ignition");
}

void startingAdvisory(){
  digitalWrite(HORN_LIGHTS_PIN, 0);
  delay(hornDelay);
  digitalWrite(HORN_LIGHTS_PIN, 1);
  delay(150);
  digitalWrite(HORN_LIGHTS_PIN, 0);
  delay(hornDelay);
  digitalWrite(HORN_LIGHTS_PIN, 1);
  delay(500);
}

void accessoryOn() {
  ACC_STATE=1;
  digitalWrite(ACC_IGNITION_PIN, 0);
  delay(250);
}

void startEngine(){
  ENGINE_STATE=1;
  digitalWrite(STARTER_PIN, 0);
  delay(2000);
  digitalWrite(STARTER_PIN, 1); 
}

void stopEngine(){
  digitalWrite(ACC_IGNITION_PIN, 1);
  digitalWrite(STARTER_PIN, 1);
  ACC_STATE=0;
  ENGINE_STATE=0;
  //client.publish("RemoteStart/Response", "shutdown");
}

void flashPushButtonLed(){
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) 
  {
    // save the last time you blinked the LED
    previousMillis = currentMillis;
    // if the LED is off turn it on and vice-versa:
    if (PUSH_START_LED_STATE == LOW) 
    {
      PUSH_START_LED_STATE = HIGH;
    } 
    else 
    {
      PUSH_START_LED_STATE = LOW;
    }
    digitalWrite(PUSH_START_LED_PIN, PUSH_START_LED_STATE);
  }
}

void fadePushButtonLed(){
  timeFade = millis();
  value = 128+127*cos(2*PI/periode*timeFade);
  value2 = 128+127*cos(2*PI/periode*(displace-timeFade));
  analogWrite(PUSH_START_LED_PIN, value2);           // sets the value (range from 0 to 255) 
}

void unlockDoors(){
  digitalWrite(DOOR_UNLOCK_PIN, 1);
  delay(500);
  digitalWrite(DOOR_UNLOCK_PIN, 0);
  delay(500);
  digitalWrite(DOOR_UNLOCK_PIN, 1);
  delay(500);
  digitalWrite(DOOR_UNLOCK_PIN, 0);
}

void lockDoors(){
  digitalWrite(DOOR_LOCK_PIN, 1);
  delay(500);
  digitalWrite(DOOR_LOCK_PIN, 0);
}
