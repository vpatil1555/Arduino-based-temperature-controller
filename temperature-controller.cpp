#include "DHT.h"//DHT sensor Library
#include <LiquidCrystal_I2C.h>//LCD Library 
#include<Wire.h>
#include <FirebaseArduino.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <ESP8266WiFi.h>


#define DHTPIN 4 //Define sensor pin(pin 12)
#define DHTTYPE DHT11 //What sensor use
#define cooler 13 //LED pins for temperature control 


DHT dht(DHTPIN, DHTTYPE);//Create sensor object
LiquidCrystal_I2C lcd(0x27,16,2);//Create lcd object using this pins 8,9,4,6,7

boolean a=LOW,b=HIGH;
float h=0,t=0;
int counter = 0;

#define WIFI_SSID "mardes home"
#define WIFI_PASSWORD "Prateek123@@"


#define projectID "warrior-bulls-default-rtdb" //Your Firebase Project ID; can be found in project settings.
#define FIREBASE_HOST "warrior-bulls-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "2JNTy9dXCRiq3Yyf78K9DqOhKZMzwnLeNIgZF5G3"


long previousMillis = 0;
long interval = 2000; //Read sensor each 2 seconds
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

void setup() {
  Serial.begin(9600);
  Wire.begin(2,0); // gpio 2 and gpio 0 which are D4, and D3

  lcd.begin();   
  lcd.backlight();          
  lcd.home(); 
  lcd.setCursor(0,0);            //Set in position 0,0

  pinMode(cooler, OUTPUT);//Change to output my pins
  digitalWrite(cooler,LOW);

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(1000);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");
  lcd.print("connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    lcd.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("connected: ");
  lcd.print("connected");
  Serial.println(WiFi.localIP());

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  timeClient.begin();
  
  dht.begin();//Start DHT22 sensor

  lcd.clear();
 lcd.print("Tempera:");// print in lcd this word 
 lcd.setCursor(0,1);//Change cursor position
 lcd.print("Humedad:");// print in lcd this word 
}

void loop()
{

  unsigned long currentMillis = millis();
  if(currentMillis - previousMillis > interval) //Comparison between the elapsed time and the time in which the action is to be executed
  {
    counter++;
    previousMillis = currentMillis; //"Last time is now"

     h = dht.readHumidity();//humidity value
     t = dht.readTemperature();//temperature value centigrades if you want farenheit change to
     //t = dht.readTemperature(true);
     //Below is for print data sensors in lcd 
     lcd.setCursor(10,0);
     lcd.print(t);
     lcd.setCursor(10,1);
     lcd.print(h);

     
    if(t>=25.00)//if temperature above of 25 degrees
    {

      Serial.println("High");
      digitalWrite(cooler,LOW); //Active air conditioner

    }
    else if(t<=23.00)
    {
     Serial.println("LOW");
     digitalWrite(cooler,HIGH);//Turn off air conditioner
     
    }

     if(counter == 10){
       timeClient.update();
       unsigned long startMillis = millis(); // Start of sample window
       char result[50];
       char resultH[50];
       
       long num = timeClient.getEpochTime();
       
       sprintf(result, "%ld/TEMP", num);
       char resultFinal[] = "RECORD/";
       strcat(resultFinal, result);
       
       //Serial.println(resultFinal);
       Firebase.setFloat(resultFinal,t);

       sprintf(resultH, "%ld/HUMIDITY", num);
       char resultFinalH[] = "RECORD/";
       strcat(resultFinalH, resultH);
       
       //Serial.println(resultFinalH);
       Firebase.setFloat(resultFinalH,h);


       lcd.clear();
       lcd.print("Updated to db");

       delay(1000);
       
       lcd.clear();
       lcd.print("Tempera:");// print in lcd this word 
       lcd.setCursor(0,1);//Change cursor position
       lcd.print("Humedad:");// print in lcd this word 

        counter = 0;
    }

    
  }

}