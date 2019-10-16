
#include <Servo.h>
#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>         // https://github.com/tzapu/WiFiManager
#include <BlynkSimpleEsp8266.h>
#include <PubSubClient.h>

Servo myservo;
BlynkTimer timer;




char mqtt_server[40]= "bstick-board.com";
char mqtt_port[6] = "9443";
char blynk_token[34] = "11b09a368feb45dfa996d529cc03de10";

//ประกาศตัวแปร
//----------------------------------
int servo = D8;
int buzzer = D7;
int swith = D1;
int led1 = D2;
int led2 = D3;
int check = 0;
//---------------------------------
#define SET_PIN 0

int blynkIsDownCount=0;

// Set web server port number to 80
WiFiServer server(80);
// Variable to store the HTTP request

// Config MQTT Server----------------------------------------------------------------------------------
#define mqtt_server "postman.cloudmqtt.com" //อย่าลืมเปลี่ยนเป็น url ของ MQTT Server ที่เราใช้นะครับ
#define mqtt_port2 15872
#define mqtt_user "fefefsqt" //mqtt user
#define mqtt_password "f3OXIJNyaG8Z" //mqtt password
 
 
WiFiClient espClient;
PubSubClient client(espClient);
//----------------------------------------------ลูปSETUP-----------------------------------------------
void setup() {
  Serial.begin(115200);
pinMode(SET_PIN, INPUT_PULLUP);
//-----------------------------------------------ไวไฟเมเนเจอร์ตั้งค่า---------------------------------------
      WiFiManagerParameter custom_blynk_token("Blynk", "Blynk", blynk_token, 32);
  WiFiManagerParameter custom_mqtt_server("mqtt_server", "mqtt_server", mqtt_server, 40);
WiFiManagerParameter custom_mqtt_port("mqtt_port", "mqtt_port", mqtt_port,6);

  // WiFiManager
  // Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;
  wifiManager.addParameter(&custom_blynk_token);
wifiManager.addParameter(&custom_mqtt_server);
wifiManager.addParameter(&custom_mqtt_port);
 

  wifiManager.autoConnect("CATTHOR");
 
  // if you get here you have connected to the WiFi
 Serial.println("Connected.....🙂");
   Serial.println("IP Address: ");
  Serial.println(WiFi.localIP());
  
  server.begin();

  Blynk.config(custom_blynk_token.getValue());
  Blynk.config(custom_mqtt_server.getValue());
  Blynk.config(custom_mqtt_port.getValue());

 if (digitalRead(SET_PIN) == LOW) {
    wifiManager.resetSettings();
  }
  Blynk.config(blynk_token, "bstick-board.com", 8080);
//------------------------------------------------------MQTTSERVER------------------------------------------
client.setServer(mqtt_server, mqtt_port2);
client.setCallback(callback);
//-----------------------------------------------------------------------------------------------------------  
 pinMode(swith, INPUT);
 pinMode(led1, OUTPUT);
 pinMode(led2, OUTPUT);
 pinMode(buzzer,OUTPUT);
 digitalWrite(led1,HIGH);
 myservo.attach(servo);
 myservo.write(250);
   delay(500);
}

void loop()//---------------------------------------------------------------<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
{ 
  
  
  
  int test = digitalRead(swith);
  Serial.println(test);
  if(test == 1){
    
    servostart();
    }
  
 
   //ส่วนการทำงานของแอพบิ้งค์
  Blynk.run();
  timer.run(); // run BlynkTimer
  //-------------------------------------------------------------------<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
  //ส่วนการเชื่อมต่อserver mqttเราเอง
  if (!client.connected()) {
    Serial.print("Attempting MQTT AUNWAconnection...");
    if (client.connect("ESP8266Client", mqtt_user, mqtt_password)) {
      Serial.println("connected");
      client.subscribe("/Server");
      
      
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
      digitalWrite(led2,HIGH);
      delay(500);
      digitalWrite(led2,LOW);
      delay(500);
      digitalWrite(led2,HIGH);
      delay(500);
      digitalWrite(led2,LOW);
      delay(500);
      return;
    }
  }
  client.loop();
  
}
//--------------------------------------------------------------------------
void servostart(){
   digitalWrite(led2,HIGH);
   sound();
   myservo.write(20);
   delay(1000);
   myservo.write(250);
   delay(1000);
   digitalWrite(led2,LOW);
   Serial.println("SERVO CAT ON");
  }
void servostart2(){
   digitalWrite(led2,HIGH);
   myservo.write(20);
   delay(1000);
   myservo.write(250);
   delay(1000);
   digitalWrite(led2,LOW);
   Serial.println("SERVO CAT ON");
  }  
void ledon(){
  digitalWrite(led1,HIGH);
  }
void sound(){
  digitalWrite(buzzer,HIGH);
  delay(500);
  digitalWrite(buzzer,LOW);
  delay(1000);
  digitalWrite(buzzer,HIGH);
  delay(1000);
  digitalWrite(buzzer,LOW);
  } 
 //----------------------------------------------------------------------------------------------------- 
void reconnecting()
{
  if (!Blynk.connected())
  {
    blynkIsDownCount++;
    BLYNK_LOG("blynk server is down! %d  times", blynkIsDownCount);
    Blynk.connect(5000);
    Blynk.syncAll();
  }

}
//--------------------------------------------------------------------------ฟังชั่นblynk--------------------
BLYNK_WRITE(V1){
     if (param.asInt()) {
        //HIGH
        ledon();
    } else {
       //LOW
      digitalWrite(led1,LOW);
    }
  }
BLYNK_WRITE(V2){
   servostart();
  
}
//--------------------------------------------------------------------------รับค่าจากserverMQTTมาทำงาน---------------
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  String msg = "";
  
  int i=0;
  while (i<length) msg += (char)payload[i++];
  if (msg == "GET") {
    client.publish("/Server", (digitalRead(led1) ? "LEDON" : "LEDOFF"));
    Serial.println("SENT TO SERVER !");
    return;
  }
  if(msg == "LEDON"){
  digitalWrite(led1,HIGH);
    }
     if(msg == "LEDOFF"){
  digitalWrite(led1,LOW);
    }   
 /* digitalWrite(led1, (msg == "LEDON" ? HIGH : LOW));
  Serial.println(msg);*/
  if(msg == "not"){
   check = 1 ;
    }
  if(msg == "sound"){
 check = 0 ;
    }  
  if(msg == "CAT")
  if(check == 0 ){
    servostart();
    }
   if(msg == "CAT")
   if(check == 1 ){
    servostart2();
    }
   if(msg == "reset"){
    digitalWrite(SET_PIN,LOW);
    delay(5000);
    }   
}
//------------------------------------------------------------------------------------------------------------------
