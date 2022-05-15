#include <WiFi.h>
#include <ThingSpeak.h>
const int trigpin = 13;
const int echopin = 12;
const char *ssid = "Redmi Note 10S";
const char *password = "Ayush123";
unsigned long myChannelNumber = 1;
const char * myWriteAPIKey = "UXMUYL9GIOW7PE3D";
WiFiClient  client;

#define SOUND_SPEED 0.034 // cm/microsecond
#define CM_TO_INCH 0.393701
#define BUZZERPIN 4
long duration;
float distanceCm;
float distanceInch;

float readDistance(){
  float distread;
  digitalWrite(trigpin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigpin,HIGH);
  delayMicroseconds(10);
  digitalWrite(trigpin, LOW);

  duration = pulseIn(echopin, HIGH);
  distread = duration * SOUND_SPEED/2;
  Serial.print("Distance: ");
  Serial.println(distread);
  return distread;
}
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("Started");
  pinMode(trigpin,OUTPUT);
  pinMode(echopin, INPUT);
  WiFi.begin(ssid, password);
  delay(5000);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
    WiFi.begin(ssid,password);
  }
  Serial.println("Connected to WiFi");
  pinMode(BUZZERPIN,OUTPUT);
  ledcSetup(0,1E5,12);
  ledcAttachPin(BUZZERPIN,0);
  WiFi.mode(WIFI_STA);
  ThingSpeak.begin(client);
}

void loop() {
    float mindist = 20.00;
    float curdist = readDistance();
   
    ThingSpeak.setField(1, curdist);
    int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
    int threshold1 = 5; 
    int threshold2 = 10;
    int threshold3 = 20;
    if(curdist <= threshold1){
      Serial.println("Object within 5cm");
      ledcWriteNote(0,NOTE_A,4);  
      delay(1000);
    }
    else if(curdist <= threshold2) {
      Serial.println("Object within 10cm");
      ledcWriteNote(0,NOTE_E,4);  
      delay(500);
    }
    else if(curdist <= threshold3) {
      Serial.println("Object within 20cm");
      ledcWriteNote(0,NOTE_C,4);  
      delay(1000);
    }
    else {
      ledcWrite(0,0);  
      delay(1000);
    }
    delay(2000);
}
