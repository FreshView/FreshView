//...................// Wifi libaries
//Firebase library
//NTPclient library
//ONE Wire temperature sensor
//
#include "IOXhop_FirebaseESP32.h"
#include "NTPClient.h"
#include "WiFiUdp.h"

//Sensor libraries
#include "OneWire.h"
#include "DallasTemperature.h"

#include <stdio.h>

//hih71200 libraries
#include "HIH61xx.h"
#include "AsyncDelay.h"


#include <Wire.h>
#include "HIH61xx.h"
#include "AsyncDelay.h"

//OLED display libraries
#include "U8g2lib.h"
#include "U8x8lib.h"

#include "U8x8lib.h"
#include <LoRa.h>

//Arducam libraries//
#include <WiFi.h>
#include <Wire.h>
#include "ESP32WebServer.h"
#include "ArduCAM.h"
#include <SPI.h>
#include "memorysaver.h"




//// The "hih" object must be created with a reference to the "Wire" object which represents the I2C bus it is using.
// Note that the class for the Wire object is called "TwoWire", and must be included in the templated class name.
HIH61xx<TwoWire> hih(Wire);

//Async delay
AsyncDelay samplingInterval;


// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

// Variables to save date and time
String formattedDate;
String dayStamp;
String timeStamp;

// Declaring sensor functions
void getTemp(void); 
void getMoisture(void);
void getLumen(void);
void getTempHumidity(void);

// firebase setup
#define FIREBASE_HOST "urban-fresh-sleep-mode-d8524.firebaseio.com"
#define FIREBASE_AUTH "4vWu1tVoipUSWOms0Cl4uoP8URQ5vdpuQOpNnsgS"
//#define WIFI_SSID "Bakery204"
//#define WIFI_PASSWORD "Electronics204"

// Sensor pins setup
#define ONE_WIRE_BUS 13
#define MoisturePin 34
#define PARpin 35

// the OLED used
U8X8_SSD1306_128X64_NONAME_SW_I2C u8x8(/* clock=*/ 15, /* data=*/ 4, /* reset=*/ 16);

//initialize all the array
char T[10];
char M[10];
char L[10];
char H[10];
char AT[10];


// Setup a oneWire instance to communicate with a OneWire device
OneWire oneWire(ONE_WIRE_BUS);
// Pass our oneWire reference to Dallas Temperature sensor 
DallasTemperature sensors(&oneWire);





#if !(defined ESP32 )
#error Please select the ArduCAM ESP32 UNO board in the Tools/Board
#endif

#define OV2640_MINI_2MP

// set GPIO23 as the slave select:
const int CS = 23;

ArduCAM myCAM(OV2640, CS);

//you can change the value of wifiType to select Station or AP mode.
//Default is AP mode.
int wifiType = 0; // 0:Station  1:AP

//AP mode configuration
//Default is arducam_esp8266.If you want,you can change the AP_aaid  to your favorite name
const char *AP_ssid = "arducam_esp32"; 
//Default is no password.If you want to set password,put your password here
const char *AP_password = NULL;

//Station mode you should put your ssid and password
const char *ssid = "Bakery204"; // Put your WIFI NAME here
const char *password = "Electronics204"; // Put your PASSWORD here

static const size_t bufferSize = 2048;
static uint8_t buffer[bufferSize] = {0xFF};
uint8_t temp = 0, temp_last = 0;
int i = 0;
bool is_header = false;

ESP32WebServer server(80);

void start_capture(void);
void camCapture(ArduCAM myCAM);
void serverCapture(void);
void handleNotFound(void);

void setup() 
{
  uint8_t vid, pid;
  uint8_t temp;
  //set the CS as an output:
  pinMode(CS,OUTPUT);
  //pinMode(CAM_POWER_ON , OUTPUT);
  // digitalWrite(CAM_POWER_ON, HIGH);

  //disable LORA_CS
  pinMode(18,OUTPUT);
  digitalWrite(18, HIGH);
  
  //#if defined(__SAM3X8E__)
  //Wire1.begin();
  //#else
  Wire.begin();
  //#endif
  Serial.begin(115200);
  Serial.println(F("ArduCAM Start!"));

  // initialize SPI:
  SPI.begin();
  SPI.setFrequency(4000000); //4MHz

  // initialize Firebse:
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);

  // Initialize a NTPClient to get time
  timeClient.begin();
  // Set offset time in seconds to adjust for your timezone, for example:
  // GMT +1 = 3600
  // GMT +8 = 28800
  // GMT -1 = -3600
  // GMT 0 = 0
  timeClient.setTimeOffset(-25200);

  //start the wire library and initialize the hih library

  Wire.begin();
  hih.initialise();
  samplingInterval.start(1000, AsyncDelay::MILLIS);

  //start OLED display libraries
  u8x8.begin();
  u8x8.setFont(u8x8_font_amstrad_cpc_extended_r);

   // Start the DallasTemperature library
   sensors.begin(); 

  Serial.println("Setup done");

  //Check if the ArduCAM SPI bus is OK
  myCAM.write_reg(ARDUCHIP_TEST1, 0x55);
  temp = myCAM.read_reg(ARDUCHIP_TEST1);
  if (temp != 0x55)
  {
    Serial.println(F("SPI1 interface Error!"));
    while(1);
  }

  //Check if the ArduCAM SPI bus is OK
  myCAM.write_reg(ARDUCHIP_TEST1, 0x55);
  temp = myCAM.read_reg(ARDUCHIP_TEST1);
  if (temp != 0x55)
  {
    Serial.println(F("SPI1 interface Error!"));
    while(1);
  }
  
  //Check if the camera module type is OV2640
  myCAM.wrSensorReg8_8(0xff, 0x01);
  myCAM.rdSensorReg8_8(OV2640_CHIPID_HIGH, &vid);
  myCAM.rdSensorReg8_8(OV2640_CHIPID_LOW, &pid);
  if ((vid != 0x26 ) && (( pid != 0x41 ) || ( pid != 0x42 )))
    Serial.println(F("Can't find OV2640 module!"));
  else
    Serial.println(F("OV2640 detected."));


  //Change to JPEG capture mode and initialize the OV2640 module
  myCAM.set_format(JPEG);
  myCAM.InitCAM();
  myCAM.OV2640_set_JPEG_size(OV2640_1280x1024);  
  
  myCAM.clear_fifo_flag();
  
  if (wifiType == 0)
  {
    if(!strcmp(ssid,"SSID"))
    {
      Serial.println(F("Please set your SSID"));
      while(1);
    }
    if(!strcmp(password,"PASSWORD"))
    {
      Serial.println(F("Please set your PASSWORD"));
      while(1);
    }
    
    // Connect to WiFi network
    Serial.println();
    Serial.println();
    Serial.print(F("Connecting to "));
    Serial.println(ssid);
    
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    
    while (WiFi.status() != WL_CONNECTED) 
    {
      delay(500);
      Serial.print(F("."));
    }
    Serial.println(F("WiFi connected"));
    Serial.println("");
    Serial.println(WiFi.localIP());
  }
  else if (wifiType == 1)
  {
    Serial.println();
    Serial.println();
    Serial.print(F("Share AP: "));
    Serial.println(AP_ssid);
    Serial.print(F("The password is: "));
    Serial.println(AP_password);
    
    WiFi.mode(WIFI_AP);
    WiFi.softAP(AP_ssid, AP_password);
    Serial.println("");
    Serial.println(WiFi.softAPIP());
  }
  
  // Start the server
  //server.on("/1", HTTP_GET, serverCapture);
  //server.onNotFound(handleNotFound);
  server.onNotFound(serverCapture);
  server.begin();
  Serial.println(F("Server started"));
}






void start_capture(void)
{
  myCAM.clear_fifo_flag();
  myCAM.start_capture();
}

void camCapture(ArduCAM myCAM)
{
  WiFiClient client = server.client();
  uint32_t len  = myCAM.read_fifo_length();
  if (len >= MAX_FIFO_SIZE) //8M
  {
    Serial.println(F("Over size."));
  }
  if (len == 0 ) //0 kb
  {
    Serial.println(F("Size is 0."));
  }
  myCAM.CS_LOW();
  myCAM.set_fifo_burst(); 
  if (!client.connected()) return;
  String response = "HTTP/1.1 200 OK\r\n";
  response += "Content-Type: image/jpeg\r\n";
  response += "Content-len: " + String(len) + "\r\n\r\n";
  server.sendContent(response);
  i = 0;
  
  while ( len-- )
    {
    temp_last = temp;
    temp =  SPI.transfer(0x00);
    //Read JPEG data from FIFO
    
    if ( (temp == 0xD9) && (temp_last == 0xFF) ) //If find the end ,break while,
    {
    buffer[i++] = temp;  //save the last  0XD9     
    //Write the remain bytes in the buffer
    if (!client.connected()) break;
    client.write(&buffer[0], i);
    is_header = false;
    i = 0;
    myCAM.CS_HIGH();
    break; 
    }  
    
    if (is_header == true)
    { 
      //Write image data to buffer if not full
      if (i < bufferSize)
        buffer[i++] = temp;
      else
      {
        //Write bufferSize bytes image data to file
        if (!client.connected()) break;
        client.write(&buffer[0], bufferSize);
        i = 0;
        buffer[i++] = temp;
      }        
    }
    else if ((temp == 0xD8) & (temp_last == 0xFF))
    {
      is_header = true;
      buffer[i++] = temp_last;
      buffer[i++] = temp;   
    } 
  } 
}

void serverCapture()
{
  delay(1000);
  start_capture();
  Serial.println(F("CAM Capturing"));
  
  int total_time = 0;
  
  total_time = millis();
  while (!myCAM.get_bit(ARDUCHIP_TRIG, CAP_DONE_MASK));
  total_time = millis() - total_time;
  Serial.print(F("capture total_time used (in miliseconds):"));
  Serial.println(total_time, DEC);
  
  total_time = 0;
  
  Serial.println(F("CAM Capture Done."));
  total_time = millis();
  camCapture(myCAM);
  total_time = millis() - total_time;
  Serial.print(F("send total_time used (in miliseconds):"));
  Serial.println(total_time, DEC);
  Serial.println(F("CAM send Done."));
}

void handleNotFound()
{
  String message = "Server is running!\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  server.send(200, "text/plain", message);
  Serial.println(message);
  
  if (server.hasArg("ql"))
  {
    int ql = server.arg("ql").toInt();
    #if defined (OV2640_MINI_2MP) || defined (OV2640_CAM)
    myCAM.OV2640_set_JPEG_size(ql);
    #endif
    
    Serial.println("QL change to: " + server.arg("ql"));
  }
}






void loop() 
{
  server.onNotFound(serverCapture);
  server.begin();
  Serial.println(F("Server started"));
  
  server.handleClient(); //Check for the server
  Serial.println("Sensors starting:");
  
   // Restarting the DallasTemperature library ensure 
   // oneWire device reads properly
   sensors.begin();
   #define ONE_WIRE_BUS 13
   
   // sensors reading
   getTemp();
   getMoisture();
   getLumen();
   getTimeStamp();
   getTempHumidity();
   updateFirebase();

   
}






void updateFirebase() 
{
  Serial.println("Uploading to Firebase");
  //update all the data
  Firebase.setString("FirebaseIOT/WaterTemp", T);
  Firebase.setString("FirebaseIOT/Moisture",M);
  Firebase.setString("FirebaseIOT/LightIntensity", L);
  Firebase.setString("DATE: ", dayStamp);
  Firebase.setString("HOUR: ", timeStamp);
  Firebase.setString("FirebaseIOT/AirTemp", AT);
  Firebase.setString("FirebaseIOT/Humidity", H);

  //log all the parameters 
  Firebase.pushString("Logs/WaterTemp",T) ;
  Firebase.pushString("Logs/Moisture", M);
  Firebase.pushString("Logs/LightIntensity", L);
  Firebase.pushString("Logs/AirTemp", AT);
  Firebase.pushString("Logs/Humidity", H);

  //if firebse uploading failed
 if (Firebase.failed()) 
  {
      Serial.print("pushing failed:");
      Serial.println(Firebase.error());  
      return;
  }
}

void getTimeStamp() 
{
  while(!timeClient.update()) 
  {
    timeClient.forceUpdate();
  }
  
  // The formattedDate comes with the following format:
  // 2018-05-28T16:00:13Z
  // We need to extract date and time
  formattedDate = timeClient.getFormattedDate();
 // Serial.println(formattedDate);

  // Extract date
  int splitT = formattedDate.indexOf("T");
  dayStamp = formattedDate.substring(0, splitT);
  Serial.print("DATE: ");
  Serial.println(dayStamp);
  Serial.print("");
  delay(1000);
  // Extract time
  timeStamp = formattedDate.substring(splitT+1, formattedDate.length()-1);
  Serial.print("HOUR: ");
  Serial.println(timeStamp);
  Serial.print("");
  delay(1000);
}

void getTempHumidity()
{ 
  float h; float at;
  bool printed = false;

  while(printed == false)
  {
    if (samplingInterval.isExpired() && !hih.isSampling()) {
      hih.start();
      printed = false;
      samplingInterval.repeat();
      Serial.println("Sampling started (using Wire library)");
    }
  
    hih.process();
  
    if (hih.isFinished() && !printed) 
    {
      printed = true;
      // Print saved values

      h = hih.getRelHumidity() / 100.0 ;
      at = hih.getAmbientTemp() / 100.0 ;
      
      sprintf(H, "%0.2f", h);
      sprintf(AT, "%0.2f", at);
      Serial.print("Humidity: "); 
      Serial.print(H);
      Serial.println(" %");
      Serial.print("AirTemp: ");
      Serial.print(AT);
      Serial.println(" deg C");
      delay(1000);

      //display the parameters on the OLED display
      u8x8.setFont(u8x8_font_amstrad_cpc_extended_r);
      u8x8.drawString(0,3, "AirTemp:");
      u8x8.setCursor(10,3);
      u8x8.print(at);
      //display the parameters on the OLED display
      u8x8.setFont(u8x8_font_amstrad_cpc_extended_r);
      u8x8.drawString(0,4, "Humidity:");
      u8x8.setCursor(10,4);
      u8x8.print(h);
    }
  }
}

void getTemp()  // DS18B20 one wire sensor reading code
{
  float t;
  sensors.requestTemperatures(); // send command to get temperature from DS18B20
  t = sensors.getTempCByIndex(0); // get water temperature (w) of the first (0) device on wire  

  sprintf(T, "%0.2f", t);
  Serial.print("WaterTemp: ");
  Serial.println(T);
  //display the parameters on the OLED display
  u8x8.setFont(u8x8_font_amstrad_cpc_extended_r);
  u8x8.drawString(0,0, "WaterTemp:");
  u8x8.setCursor(10,0);
  u8x8.print(t);
}

void getMoisture()
{
  float m;
  int analogIn = analogRead(MoisturePin); //reading the analog input from the moisture sensor
  m = map(analogIn, 0, 1023, 100, 0);   // converting the raw analog reading into a percentage based on rockwool 
  
  sprintf(M, "%0.2f", m);
  Serial.print("moisture= ");
  Serial.println(M);
  //display the parameters on the OLED display
  u8x8.setFont(u8x8_font_amstrad_cpc_extended_r);
  u8x8.drawString(0,1, "Moisture:");
  u8x8.setCursor(9,1);
  u8x8.print(m);
}

void getLumen()
{
  float l;
  int analogIn = analogRead(PARpin);    // reading the analog input from the photoresistor voltage divider
  l = map(analogIn, 0, 1023, 100, 0);   // converting the raw analog reading into a percentage

  sprintf(L, "%0.2f", l);
  Serial.print("Photoresistor percentage= ");
  Serial.println(L);
  //display the parameters on the OLED display
  u8x8.setFont(u8x8_font_amstrad_cpc_extended_r);
  u8x8.drawString(0,2, "light:");
  u8x8.setCursor(9,2);
  u8x8.print(l);
}
