#include <Servo.h>
#include <WiFi.h>
#include <ThingSpeak.h>

#define trigger 27
#define echo 13

#define redled 22
#define yellowled 23

#define Soundspeed 0.034
#define servopin 32

#define parkingthreshold 50
#define tempthreshold 50
//#define tempthreshold 20

Servo myservo;
WiFiClient wificlient;

int status = WL_IDLE_STATUS;
#define WIFI_TIMEOUT 10000



int channelnum= 1569795;

float d1,d2,d3;
float temp;

//char* ssid="OnePlus 8 Pro";
//char* pwd = "givemedata";
const char* ssid="FTTHbsnl";
const char* pwd="anil2501";

long duration;
float distance;
int angle=90;

void connectwifi()
{
  Serial.println("Connecting to WiFi");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pwd);

  unsigned long startAttemptTime = millis();
  
  while(WiFi.status() != WL_CONNECTED)
  {
    // Connection timed out
    if(millis() - startAttemptTime >= WIFI_TIMEOUT)
    {
      Serial.println();
      Serial.println("Connection timed out");
      break;
    }

    // Loading
    Serial.print(".");
    delay(200);
  }

  // Failed to connect
  if(WiFi.status() != WL_CONNECTED)
  {
    Serial.println("Failed to connect to WiFi Network");
    return;
  }

  Serial.println("Connected!");
  Serial.println(WiFi.localIP());
}
 
void setup() {
  Serial.begin(9600);
  connectwifi();
  pinMode(trigger,OUTPUT);
  pinMode(echo, INPUT);
  myservo.attach(32);
  pinMode(redled,OUTPUT);
  pinMode(yellowled,OUTPUT);
  digitalWrite(redled,LOW);
  digitalWrite(yellowled,LOW);
  ThingSpeak.begin(wificlient);
  // put your setup code here, to run once:
}

void loop() {
  // put your main code here, to run repeatedly:
 
  // read values from thingspeak here
  d1=ThingSpeak.readFloatField(channelnum,1);
  d2=ThingSpeak.readFloatField(channelnum,2);
  d3=ThingSpeak.readFloatField(channelnum,3);
  temp=ThingSpeak.readFloatField(channelnum,4);
  Serial.println(d1);
  Serial.println(d2);
  Serial.println(d3);
  Serial.println(temp);
  digitalWrite(trigger,LOW);
  delayMicroseconds(5);
  digitalWrite(trigger, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigger,LOW);
  duration=pulseIn(echo, HIGH);
  distance = duration * Soundspeed/2;
  Serial.print("Distance is: ");
  Serial.print(distance);
  Serial.println("cm");
  if(temp >tempthreshold)
      {
        digitalWrite(redled,HIGH);
        //Serial.println("Fire warning");
      }
  if(d1>parkingthreshold || d2>parkingthreshold || d3>parkingthreshold) //slot is available
  {
    digitalWrite(yellowled,LOW);
    if(distance < 10.00)
    {
      //use thingspeak values for comparison here
        if(temp >tempthreshold)
        {
          digitalWrite(redled,HIGH);
          //digitalWrite(yellowled,LOW);
          Serial.println("Fire warning");
        }
        else
        {
          digitalWrite(redled,LOW);
          digitalWrite(yellowled,LOW);
          myservo.write(90);
          Serial.println("Gate opening");
          delay(10000); //gate is opened for 10 seconds
          myservo.write(10);
        }
    }
  }
  else
  {
    if(temp >tempthreshold)
    {
      digitalWrite(redled,HIGH);
      Serial.println("Fire warning");
    }
    digitalWrite(yellowled,HIGH);
    Serial.println("No slots empty");
  }
  Serial.println("Next session");
  delay(200);
}