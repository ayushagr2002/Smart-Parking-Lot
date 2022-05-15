#include <WiFi.h>
//#include <ESPAsyncWebServer.h>
#include "HTTPClient.h"
#include "time.h"
#include <ArduinoJson.h>
#include <ThingSpeak.h>
#include <Adafruit_BMP085.h>

Adafruit_BMP085 bmp;

const int trigpin = 33;
const int echopin = 32;
const char *ssid = "jigneshshah@06";
const char *password = "vishwa@304";
String cse_ip = "192.168.1.7";
String cse_port = "8080";
String server = "http://" + cse_ip + ":" + cse_port + "/~/in-cse/in-name/";
String ae = "Ultrasonic_Sensor";
String cnt = "node1";

unsigned long myChannelNumber = 1;
const char * myWriteAPIKey = "UXMUYL9GIOW7PE3D";

WiFiClient  client;

//AsyncWebServer server(80);
#define SOUND_SPEED 0.034 // cm/microsecond
#define CM_TO_INCH 0.393701
#define BUZZERPIN 27

long duration;
float distanceCm;
float distanceInch;
void createCI(String& val){
// add the lines in step 3-6 inside this function
  ae = "Ultrasonic_Sensor";
  HTTPClient http;
  http.begin(server + ae + "/" + cnt + "/");
  http.addHeader("X-M2M-Origin", "admin:admin");
  http.addHeader("Content-Type", "application/json;ty=4");
  int code = http.POST("{\"m2m:cin\": {\"cnf\":\"application/json\",\"con\": " + String(val) + "}}");
  Serial.println(code);
  if (code == -1) {
    Serial.println("UNABLE TO CONNECT TO THE SERVER");
  }
  http.end();
}

void createCII(String& val){
// add the lines in step 3-6 inside this function
  HTTPClient http;
  ae = "Temperature_Sensor";
  http.begin(server + ae + "/" + cnt + "/");
  http.addHeader("X-M2M-Origin", "admin:admin");
  http.addHeader("Content-Type", "application/json;ty=4");
  int code = http.POST("{\"m2m:cin\": {\"cnf\":\"application/json\",\"con\": " + String(val) + "}}");
  Serial.println(code);
  if (code == -1) {
    Serial.println("UNABLE TO CONNECT TO THE SERVER");
  }
  http.end();
}

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

float readTemp(){
  Serial.print("Temperature = ");
  float temp = bmp.readTemperature();
    Serial.print(temp);
    Serial.println(" *C");
    return temp;
}

/*
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.7.2/css/all.css" integrity="sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr" crossorigin="anonymous">
  <style>
    html {
     font-family: Arial;
     display: inline-block;
     margin: 0px auto;
     text-align: center;
    }
    h2 { font-size: 3.0rem; }
    p { font-size: 3.0rem; }
    .readinglabel{
      font-size: 1.5rem;
      vertical-align:middle;
      padding-bottom: 15px;
    }
  </style>
</head>
<body>
  <h2>Smart Parking lot</h2>
  <p>
    <span class="readinglabel">Distance (Slot 1): </span> 
    <span id="distance1">%DISTANCE1%</span>
  </p>
  <p>
    <span class="readinglabel">Distance (Slot 2): </span> 
    <span id="distance2">%DISTANCE2%</span>
  </p>
  <p>
    <span class="readinglabel">Distance (Slot 3): </span> 
    <span id="distance3">%DISTANCE3%</span>
  </p>
</body>
<script>
setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("distance1").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/distance1", true);
  xhttp.send();
}, 5000 ) ;
</script>
</html>)rawliteral";

String processor(const String& var){
  //Serial.println(var);
  if(var == "DISTANCE1"){
    return String(readDistance());
  }
  return String();
}
*/

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(trigpin,OUTPUT);
  pinMode(echopin, INPUT);
  WiFi.begin(ssid, password);
  delay(5000);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  
  Serial.println(WiFi.localIP());
  server.begin();
  /*server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });
  server.on("/distance1", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", String(readDistance()).c_str(), processor);
  });eshshah@06";
const char *password = "vishwa@304";
  */
  pinMode(27,OUTPUT);
  ledcSetup(0,1E5,12);
  ledcAttachPin(BUZZERPIN,0);
  WiFi.mode(WIFI_STA);
  
  ThingSpeak.begin(client);
  while (!bmp.begin()) {
    Serial.println("Connecting with BMP... ");
  }
  // server.begin();
}

void loop() {
    float mindist = 20.00;
    float curdist = readDistance();
    String val = (String)curdist;
    createCI(val);
    float curtemp = readTemp();
    val = (String)curtemp;
    createCII(val);
    int x = ThingSpeak.writeField(myChannelNumber,3,curdist, myWriteAPIKey);
    int y = ThingSpeak.writeField(myChannelNumber,4,curtemp, myWriteAPIKey);
//    while(curdist< 20.00)
//    {
//      Serial.println("Car too close");
//      ledcWriteTone(0,800);
//      delay(2000);
//      curdist = readDistance();
//    }
//    ledcWriteTone(0,0);
    int threshold1 = 5; 
    int threshold2 = 10;
    int threshold3 = 20;
    while(curtemp > 28.00) {
      curtemp = readTemp();
      ThingSpeak.writeField(myChannelNumber,4,curtemp, myWriteAPIKey);
      Serial.println(NOTE_B);
      ledcWriteNote(0,NOTE_B,4);  
    }
    if(curdist <= threshold1){
      Serial.println("Object within 5cm");
      //SerialBT.println("Object within 5cm");
      /*ledcWriteTone(0,800);
      delay(1000);
      uint8_t octave = 1;*/
      ledcWriteNote(0,NOTE_A,4);  
      delay(100);
    }
    else if(curdist <= threshold2) {
      Serial.println("Object within 10cm");
      //SerialBT.println("Object within 10cm");
      /*ledcWriteTone(0,800);
      delay(1000);
      uint8_t octave = 1;*/
      ledcWriteNote(0,NOTE_E,4);  
      delay(500);
    }
    else if(curdist <= threshold3) {
      Serial.println("Object within 20cm");
      //SerialBT.println("Object within 20cm");
      /*ledcWriteTone(0,800);
      delay(1000);
      uint8_t octave = 1;*/
      ledcWriteNote(0,NOTE_C,4);  
      delay(100);
    }
    else {
      ledcWrite(0,0);  
      delay(100);
    }
    delay(100);
}
