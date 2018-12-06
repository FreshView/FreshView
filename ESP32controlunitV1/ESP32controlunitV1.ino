
//...................// Wifi libaries
#include <WiFi.h>
#include "IOXhop_FirebaseESP32.h"
#include <stdio.h>
//OLED display libraries
#include <Wire.h>
//include all the libraries for OLED display
#include <U8g2lib.h>
#include <U8x8lib.h>

#include <U8x8lib.h>
#include <LoRa.h>

// Variables to save date and time
String formattedDate;
String dayStamp;
String timeStamp;

// Declaring sensor functions
void getTemp(void); 
void getMoisture(void);
void getLumen(void);
void getTempHumidity(void);
//void getDesiredTemp(void);
//void getDesiredMoistr(void);
//void getdata(void);
// Wifi setup
#define FIREBASE_HOST "urban-fresh-sleep-mode-d8524.firebaseio.com"
#define FIREBASE_AUTH "4vWu1tVoipUSWOms0Cl4uoP8URQ5vdpuQOpNnsgS"
#define WIFI_SSID "Bakery204"
#define WIFI_PASSWORD "Electronics204"

// Sensor pins setup
//#define ONE_WIRE_BUS 13
//#define MoisturePin 34
//#define PARpin 35

//define pins 
#define Fan 12
#define Pump 27
#define Light 26

//#define gpio_num_12 = 12

// the OLED used
U8X8_SSD1306_128X64_NONAME_SW_I2C u8x8(/* clock=*/ 15, /* data=*/ 4, /* reset=*/ 16);

 //define all the character arrays for the functions 
char T[10];
char M[10];
char L[10];
char H[10];
char AT[10];
char Dtemp[10];
char Dmoistr[10];

void setup() 
{ //define pins as output
  pinMode(Fan, OUTPUT);
  pinMode(Pump, OUTPUT);
  pinMode(Light, OUTPUT);

  Serial.begin(9600);
 // delay(1000);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  //Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED) 
  {
  //  Serial.print(".");
   delay(500);
  }
  
  Serial.println();
 // Serial.print("connected: ");
 // Serial.println(WiFi.localIP());
  
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);


  Wire.begin();

  u8x8.begin();
  u8x8.setFont(u8x8_font_amstrad_cpc_extended_r);


}

void loop() 
{

//void getDesiredTemp()
// {//char DAT[10];
  float setTempF, AirTempF; //define float 

  String setTemp = Firebase.getString("/FirebaseIOT/DesiredTemp");  //get strings from the firebase for temp
  setTemp.remove(0,2);
 // Serial.println(setTemp);
  setTempF  = setTemp.toFloat(); //convert from float to string
  sprintf(Dtemp,"%0.2f", setTempF); 
  Serial.print("Recieved temp:");
  Serial.println(Dtemp);
  delay(100);

  String AirTemp = Firebase.getString("/FirebaseIOT/AirTemp"); //get string form firebase
 // AirTemp.remove(0,2);
  AirTempF  = AirTemp.toFloat();
  Serial.print("AirTemp:");
  Serial.println(AirTempF);
  delay(100);
 
  if( AirTempF > setTempF ) //set conditions to set the fan HIGH
   {
    Serial.println("Fan HIGH");
    digitalWrite(Fan, HIGH);
    //Firebase.Set
    }
     if( AirTempF < setTempF) //set conditions to set low
    {
    Serial.println("Fan LOW");
    digitalWrite(Fan, LOW);
  //Firebase.Set
   }
//   else
//   {
//    Serial.println("Fan LOW");
//    digitalWrite(Fan, LOW);
//   }



   
 //}


//void getDesiredMoistr()
 //{//  char DM[10];
  float setMoistureF, MoistureF;  //define float

  String setMoisture = Firebase.getString("/FirebaseIOT/DesiredMoisture"); //get string from the firebase
  setMoisture.remove(0,2); //remove the last two characters from the recieved string
  setMoistureF  = setMoisture.toFloat();
  sprintf(Dmoistr, "%0.2f", setMoistureF);
  Serial.print("DesiredMoisture:");
  Serial.println(Dmoistr);
  delay(100);

  String Moisture = Firebase.getString("/FirebaseIOT/Moisture"); //get string from the firebase
  MoistureF  = Moisture.toFloat();
  Serial.print("Moisture:");
  Serial.println(MoistureF);
  delay(100);

  
  if( MoistureF > setMoistureF)  //set conditions to set the pump low
  {
    Serial.println("Pump LOW");
    digitalWrite(Pump, LOW);
    // Firebase.Set
    }
   if( MoistureF < setMoistureF)   //set conditions to set the pump HIGH
  {
    Serial.println("Pump HIGH");
    digitalWrite(Pump, HIGH);
    // Firebase.Set
  }
//  else
//    {
//      digitalWrite(13, HIGH);
//    }


//}



 String WaterTemp = Firebase.getString("/FirebaseIOT/WaterTemp"); //get string from the firebase 
 //String WaterTemp = WaterTemperature.remove(0,2);
 Serial.print("WaterTemp:");
 Serial.println(WaterTemp);
 u8x8.setFont(u8x8_font_amstrad_cpc_extended_r); //define the font for display
 u8x8.drawString(0,0, "WaterTemp:"); //print the string on dispaly
 u8x8.setCursor(10,0);
 u8x8.print(WaterTemp);

 //delay(100);
 u8x8.setFont(u8x8_font_amstrad_cpc_extended_r); //define the font for display
 u8x8.drawString(0,1, "Moisture:");//print the string on dispaly
 u8x8.setCursor(9,1);
 u8x8.print(Moisture);

 //delay(100);
 u8x8.setFont(u8x8_font_amstrad_cpc_extended_r); //define the font for display
 u8x8.drawString(0,2, "DMoistr:");//print the string on dispaly
 u8x8.setCursor(9,2);
 u8x8.print(Dmoistr);

// delay(100);
 String Humidity = Firebase.getString("/FirebaseIOT/Humidity");
 //String WaterTemp = WaterTemperature.remove(0,2);
 Serial.print("Humidity:");
 Serial.println(Humidity);
 u8x8.setFont(u8x8_font_amstrad_cpc_extended_r); //define the font for display
 u8x8.drawString(0,3, "Humidity:");//print the string on dispaly
 u8x8.setCursor(10,3);
 u8x8.print(Humidity);

 //delay(100);
 String LightIntensity = Firebase.getString("/FirebaseIOT/LightIntensity");
 //String WaterTemp = WaterTemperature.remove(0,2);
 Serial.print("Light:");
 Serial.println(LightIntensity);
 u8x8.setFont(u8x8_font_amstrad_cpc_extended_r); //define the font for display
 u8x8.drawString(0,4, "Light:");//print the string on dispaly
 u8x8.setCursor(8,4);
 u8x8.print(LightIntensity);

 //delay(100);
 // String AirTemp = Firebase.getString("/FirebaseIOT/AirTemp");
 //String WaterTemp = WaterTemperature.remove(0,2);
 Serial.print("AirTemp:");
 Serial.println(AirTemp);
 u8x8.setFont(u8x8_font_amstrad_cpc_extended_r); //define the font for display
 u8x8.drawString(0,5, "AirTemp:");//print the string on dispaly
 u8x8.setCursor(10,5);
 u8x8.print(AirTemp);
 
 //delay(100);
 u8x8.setFont(u8x8_font_amstrad_cpc_extended_r); //define the font for display
 u8x8.drawString(0,6, "DTemp:");//print the string on dispaly
 u8x8.setCursor(10,6);
 u8x8.print(Dtemp);

 float LightF;
 
 String Lights = Firebase.getString("/FirebaseIOT/Lights");
 Lights.remove(0,2);
 LightF = Lights.toFloat();
 Serial.print("Lights:");
 Serial.println(LightF);

  if( LightF == 0  )  
  {
    Serial.println("Lights LOW");
    digitalWrite(Light, LOW);
    // Firebase.Set
    }
   if(LightF == 1 )  
  {
    Serial.println("Lights HIGH");
    digitalWrite(Light, HIGH);
    // Firebase.Set
  }

 

 if (Firebase.failed()) 
  {
      Serial.print("pushing failed:");
      Serial.println(Firebase.error());  
      return;
  }

 
// getTemp();
// getMoisture();
// getLumen();
// getTimeStamp();
// getTempHumidity();
// updateFirebase();
// getDesiredTemp();
// getDesiredMoistr();
}





//void updateFirebase() 
//{
// // float H, AT;
// // float t, m, l;
//  //char T;
//  //T=getTemp();
////  H = getTempHumidity();
// // AT = getTempHumidity();
//
//  
////  getMoisture();
////  getLumen();
////  getTemp(); //passes pointer of string for function to write to
// 
//  
//  //Serial.print(T);
//  //t = getTemp();
//  //m = getMoisture();
//  //l = getLumen();
//
//  
//
//  Firebase.setString("FirebaseIOT/WaterTemp", T);
//  //Firebase.setFloat("LiveTemperature", t);
//  Firebase.setString("FirebaseIOT/Moisture",M);
//  Firebase.setString("FirebaseIOT/LightIntensity", L);
//  Firebase.setString("DATE: ", dayStamp);
//  Firebase.setString("HOUR: ", timeStamp);
//  Firebase.setString("FirebaseIOT/AirTemp", AT);
//  Firebase.setString("FirebaseIOT/Humidity", H);
//  
//   // Firebase.setString(M, T);
////  Firebase.setString("LiveData/Temperature", T);
////  //Firebase.setFloat("LiveTemperature", t);
////  Firebase.setString("LiveData/Moisture",M);
////  Firebase.setString("LiveData/LightIntensity", L);
////  Firebase.setString("LiveData/DATE: ", dayStamp);
////  Firebase.setString("LiveData/HOUR: ", timeStamp);
//
//  Firebase.pushString("Logs/WaterTemp",T) ;
//  Firebase.pushString("Logs/Moisture", M);
//  Firebase.pushString("Logs/LightIntensity", L);
//  Firebase.pushString("Logs/AirTemp", AT);
//  Firebase.pushString("Logs/Humidity", H);
////  Firebase.pushString("Logs/DATE: ", dayStamp);
////  Firebase.pushString("Logs/HOUR: ", timeStamp);
// 
//
// 
// if (Firebase.failed()) 
//  {
//      Serial.print("pushing failed:");
//      Serial.println(Firebase.error());  
//      return;
//  }
//}

//void getTimeStamp() {
// while(!timeClient.update()) {
//    timeClient.forceUpdate();
//  }
//  // The formattedDate comes with the following format:
//  // 2018-05-28T16:00:13Z
//  // We need to extract date and time
//  formattedDate = timeClient.getFormattedDate();
// // Serial.println(formattedDate);
//
//  // Extract date
//  int splitT = formattedDate.indexOf("T");
//  dayStamp = formattedDate.substring(0, splitT);
//  Serial.print("DATE: ");
//  Serial.println(dayStamp);
//  Serial.print("");
//  delay(1000);
//  // Extract time
//  timeStamp = formattedDate.substring(splitT+1, formattedDate.length()-1);
//  Serial.print("HOUR: ");
//  Serial.println(timeStamp);
//  Serial.print("");
//  delay(1000);
//}

//void getTempHumidity()
//{ 
//  float h; float at;
//  bool printed = false;
//
//  while(printed == false)
//  {
//    if (samplingInterval.isExpired() && !hih.isSampling()) {
//      hih.start();
//      printed = false;
//      samplingInterval.repeat();
//      Serial.println("Sampling started (using Wire library)");
//    }
//  
//    hih.process();
//  
//    if (hih.isFinished() && !printed) {
//      printed = true;
//      // Print saved values
//
//      h = hih.getRelHumidity() / 100.0 ;
//      at = hih.getAmbientTemp() / 100.0 ;
//      
//      sprintf(H, "%0.2f", h);
//      sprintf(AT, "%0.2f", at);
//      Serial.print("Humidity: "); 
//    //  Serial.print(hih.getRelHumidity() / 100.0);
//      Serial.print(H);
//      Serial.println(" %");
//      Serial.print("AirTemp: ");
//      //Serial.print(hih.getAmbientTemp() / 100.0);
//      Serial.print(AT);
//      Serial.println(" deg C");
//      delay(1000);
//    //  Serial.print("Status: ");
//    //  Serial.println(hih.getStatus());
//
//   u8x8.setFont(u8x8_font_amstrad_cpc_extended_r);
//   u8x8.drawString(0,3, "AirTemp:");
//   u8x8.setCursor(10,3);
//   u8x8.print(at);
//
//   u8x8.setFont(u8x8_font_amstrad_cpc_extended_r);
//   u8x8.drawString(0,4, "Humidity:");
//   u8x8.setCursor(10,4);
//   u8x8.print(h);
//    }
//  }
//}




//void getTemp()  // DS18B20 one wire sensor reading code
//{
//  float t;
//  //char T[10];
//  //int n;
//  sensors.requestTemperatures(); // send command to get temperature from DS18B20
//  t = sensors.getTempCByIndex(0); // get water temperature (w) of the first (0) device on wire  
//
//  sprintf(T, "%0.2f", t);
//  Serial.print("WaterTemp: ");
// // Serial.println(T);
//  // debugging
// // Serial.print("temperature: ");
// // Serial.println(t);
// // return t; // end the function and return the temperature reading
//   Serial.println(T);
////  return T;
//
// u8x8.setFont(u8x8_font_amstrad_cpc_extended_r);
// u8x8.drawString(0,0, "WaterTemp:");
// u8x8.setCursor(10,0);
// u8x8.print(t);
//
//}

//void getMoisture()
//{
//  float m;
//  int analogIn = analogRead(MoisturePin); //reading the analog input from the moisture sensor
//  m = map(analogIn, 0, 1023, 100, 0);   // converting the raw analog reading into a percentage based on rockwool 
//  
//  sprintf(M, "%0.2f", m);
//  // debuggin code  
//  //Serial.println(anlogIn);
//  Serial.print("moisture= ");
//  Serial.print(M);
//  Serial.println("%");
//
// u8x8.setFont(u8x8_font_amstrad_cpc_extended_r);
// u8x8.drawString(0,1, "Moisture:");
// u8x8.setCursor(9,1);
// u8x8.print(m);
//
//  //return m; // ending the function and returning the moisture reading
//}

//void getLumen()
//{
//  float l;
//  int analogIn = analogRead(PARpin);    // reading the analog input from the photoresistor voltage divider
//  l = map(analogIn, 0, 1023, 100, 0);   // converting the raw analog reading into a percentage
//
//  sprintf(L, "%0.2f", l);
//  // debugging code
//  Serial.print("Photoresistor percentage= ");
//  Serial.println(L);
//  
// u8x8.setFont(u8x8_font_amstrad_cpc_extended_r);
// u8x8.drawString(0,2, "light:");
// u8x8.setCursor(10,2);
// u8x8.print(l);
//  //return l;
//}



//void getdata()
// {//char DAT[10];
////  char DM[10];
//  float setTempF, setMoistureF;
//
//  String setTemp = Firebase.getString("/FirebaseIOT/DesiredTemp");
//  setTemp.remove(0,2);
//  setTempF  = setTemp.toFloat();
//  sprintf(DAT, "%0.2f", setTempF);
//  Serial.print("Recieved temp:");
//  Serial.println(DAT);
//  //delay(100);
//// u8x8.setFont(u8x8_font_amstrad_cpc_extended_r);
//// u8x8.drawString(0,2, "light:");
//// u8x8.setCursor(10,2);
//// u8x8.print(l);
//
//
//  String setMoisture = Firebase.getString("/FirebaseIOT/DesiredMoisutre");
//  setMoisture.remove(0,2);
//  setMoistureF  = setMoisture.toFloat();
//  sprintf(DM, "%0.4f", setMoistureF);
//  Serial.print("Recieved Moisture:");
//  Serial.println(setMoistureF);
//  Serial.println(DM);
//  //Serial.println(setMoisture);
//  delay(100);
//  
//
//}
