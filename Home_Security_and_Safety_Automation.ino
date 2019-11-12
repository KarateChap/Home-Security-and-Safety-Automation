#define SS_PIN 9
#define RST_PIN 8
#define SP_PIN 8
#define FAN 13
#define RED 5
#define GREEN 6
#define BLUE 7
#define SOUND 23
#define LOCK 22

#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include<SoftwareSerial.h>
#include "SPI.h"
#include "MFRC522.h"
#include <SoftwareSerial.h>

LiquidCrystal_I2C lcd(0x27, 2,1,0,4,5,6,7,3, POSITIVE);
MFRC522 rfid(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;
SoftwareSerial mySerial(3, 2);

int trigPin = 10;
int echoPin = 11; 
long duration;
int distance;
int respin = A5;
int resval = 0;
int DO = 12; 
int sensorPin = A0; 
int sensorValue = 0; 
int count = 0;
int fanActivation = 1;
void setup() {
  Serial.begin(9600);
  SPI.begin();
  rfid.PCD_Init();
  lcd.begin(16,2);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(DO, INPUT); 
  pinMode(FAN, OUTPUT);
  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(BLUE, OUTPUT);
  pinMode(SOUND, OUTPUT);
  pinMode(LOCK, OUTPUT);
  digitalWrite(BLUE, HIGH);
  mySerial.begin(9600);
}
void loop() {
  if(fanActivation == 1){
    digitalWrite(FAN, HIGH);
    fanActivation = 0;
  }
  RFID();
  ULTRASONIC();
  WATER();
  GAS();
  FLAME();
}
/**
        Author: IOXhop
        Date: N.D.
        Description: This code scans the RFID Hexadecimal Tag and confirms if the tag is authorize or not.
**/
void RFID(){
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial())
    return;
  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
  if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&
    piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
    piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
    Serial.println(F("Your tag is not of type MIFARE Classic."));
    return;
  }
  String strID = "";
  for (byte i = 0; i < 4; i++) {
    strID +=
    (rfid.uid.uidByte[i] < 0x10 ? "0" : "") +
    String(rfid.uid.uidByte[i], HEX) +
    (i!=3 ? ":" : "");
  }
  strID.toUpperCase();
  Serial.print("Tap card key: ");
  Serial.println(strID);

  if (strID.indexOf("97:C4:35:1B") >= 0 || strID.indexOf("E2:17:C3:1B") >= 0){
    fanActivation = 1;
    lcd.print("ACCESS GRANTED!");
    digitalWrite(LOCK, HIGH);
    digitalWrite(BLUE, LOW);
    digitalWrite(RED, LOW);
    digitalWrite(GREEN, HIGH);
    tone(SOUND, 2000);
    delay(100);
    noTone(SOUND);
    delay(50);
    tone(SOUND, 2000);
    delay(100);
    noTone(SOUND);
    
    delay(5000);
    digitalWrite(LOCK, LOW);
    lcd.clear();
    digitalWrite(BLUE, HIGH);
    digitalWrite(RED, LOW);
    digitalWrite(GREEN, LOW);  
    count = 0;
    delay(5000);
    
  }
  else{
    count++;
    if(count == 3){
    lcd.print("Intruder Alert!");

    /**
        Author: Last Minute Engineers 
        Date: N.D
        Description: This code will initialize text message to the members of the family whenever it detects hazard or unwanted guest
**/
    mySerial.println("AT");
    updateSerial();
    mySerial.println("AT+CMGF=1");
    updateSerial();
    mySerial.println("AT+CMGS=\"+639125299027\"");
    updateSerial();
    mySerial.print("Anak, umuwi ka muna sa bahay, mag nagtatangkang pumasok sa pintuan natin!");
    updateSerial();
    mySerial.write(26);
    delay(3000);
    count = 0;
    }
    else{
      lcd.print("ACCESS DENIED!");
    }
    digitalWrite(FAN, LOW);
    digitalWrite(BLUE, LOW);
    digitalWrite(RED, HIGH);
    digitalWrite(GREEN, LOW);
    tone(SOUND, 100);
    delay(500);
    noTone(SOUND);

    delay(3000);
    lcd.clear();
    digitalWrite(BLUE, HIGH);
    digitalWrite(RED, LOW);
    digitalWrite(GREEN, LOW);  
  }
  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
}
/**
        Author: How To Mechatronics
        Date: July 26, 2015
        Description: This code detects is there is an unwanted guest that will enter the window through sonar or sound waves.
**/
void ULTRASONIC(){
  digitalWrite(trigPin, LOW);
  delay(50);
  digitalWrite(trigPin, HIGH);
  delay(50);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);
  distance = duration;

  if(distance < 500){
    lcd.print("Intruder Alert!");
    digitalWrite(FAN, LOW);
    digitalWrite(BLUE, LOW);
    digitalWrite(RED, HIGH);
    digitalWrite(GREEN, LOW);
    tone(SOUND, 100);
    delay(500);
    noTone(SOUND);
    /**
        Author: Last Minute Engineers 
        Date: N.D
        Description: This code will initialize text message to the members of the family whenever it detects hazard or unwanted guest
**/
    mySerial.println("AT");
    updateSerial();
    mySerial.println("AT+CMGF=1");
    updateSerial();
    mySerial.println("AT+CMGS=\"+639397377889\"");
    updateSerial();
    mySerial.print("Anak, umuwi ka muna sa bahay, mag nagtatangkang pumasok sa bintana natin!");
    updateSerial();
    mySerial.write(26);
    
    
    delay(3000);
    lcd.clear();
    digitalWrite(BLUE, HIGH);
    digitalWrite(RED, LOW);
    digitalWrite(GREEN, LOW); 

   
  }
   Serial.println(distance);
}
/**
        Author: The geek Pub
        Date: May 12, 2019
        Description: This code detects the water level that it can measure from Low, Medium, and High level.
**/
void WATER(){
  resval = analogRead(respin);
  if (resval > 100 && resval < 230){
    tone(SOUND, 1000);
    delay(200);
    noTone(SOUND);
    lcd.print("Water Level: Low");
    delay(2000);
    
    lcd.clear();
    lcd.print("Safe to Stay..");
    delay(2000);
    lcd.clear();
  }
  else if (resval > 230 && resval < 250){
    digitalWrite(FAN, LOW);
    tone(SOUND, 1000);
    delay(200);
    noTone(SOUND);
    lcd.print("Water Level is");
    delay(2000);
    lcd.clear();
    lcd.print("Medium");
    delay(2000);
    lcd.clear();
    lcd.print("Prepare to");
    delay(2000);
    lcd.clear();
    lcd.print("Evacuate");
    delay(2000);
    lcd.clear();
  }
  else if (resval > 250){
    digitalWrite(FAN, LOW);
    tone(SOUND, 1000);
    delay(200);
    noTone(SOUND);
    lcd.print("Water Level is");
    delay(2000);
    lcd.clear();
    lcd.print("High");
    delay(2000);
    lcd.clear();
    lcd.print("EVACUATE NOW!");
    digitalWrite(BLUE, LOW);
    digitalWrite(RED, HIGH);
    digitalWrite(GREEN, LOW);
    tone(SOUND, 100);
    delay(500);
    noTone(SOUND);
    /**
        Author: Last Minute Engineers 
        Date: N.D
        Description: This code will initialize text message to the members of the family whenever it detects hazard or unwanted guest
**/
    mySerial.println("AT");
    updateSerial();
    mySerial.println("AT+CMGF=1");
    updateSerial();
    mySerial.println("AT+CMGS=\"+639125299027\"");
    updateSerial();
    mySerial.print("Anak, mataas na ang lebel ng tubig sa bahay lumikas na tayo!");
    updateSerial();
    mySerial.write(26);
    
    
    delay(3000);
    lcd.clear();
    digitalWrite(BLUE, HIGH);
    digitalWrite(RED, LOW);
    digitalWrite(GREEN, LOW); 
  }
  //Serial.println(resval);
  delay(50);
}
/**
        Author: ElectroPeak
        Date: April 10, 2019
        Description: This code will allow the MQ-9 GAS sensor to detect Carbon monoxide and Flammable gases.
**/
void GAS(){
 int alarm = 0; 
 float sensor_volt; 
 float RS_gas; 
 float ratio; 
 float R0 = -0.10; 
 int sensorValue = analogRead(A0); 
 sensor_volt = ((float)sensorValue / 1024) * 5.0; 
 RS_gas = (5.0 - sensor_volt) / sensor_volt; 
 ratio = RS_gas / R0;
 alarm = digitalRead(DO); 
 if (alarm == 0) {
  digitalWrite(FAN, LOW);
  lcd.print("Gas Detected!");
  digitalWrite(BLUE, LOW);
    digitalWrite(RED, HIGH);
    digitalWrite(GREEN, LOW);
    tone(SOUND, 100);
    delay(500);
    noTone(SOUND);
    /**
        Author: Last Minute Engineers 
        Date: N.D
        Description: This code will initialize text message to the members of the family whenever it detects hazard or unwanted guest
**/
    mySerial.println("AT");
    updateSerial();
    mySerial.println("AT+CMGF=1");
    updateSerial();
    mySerial.println("AT+CMGS=\"+639397377889\"");
    updateSerial();
    mySerial.print("Anak, umuwi ka muna, nag leleak ang GAS natin!");
    updateSerial();
    mySerial.write(26);
    
    
    delay(3000);
    lcd.clear();
    digitalWrite(BLUE, HIGH);
    digitalWrite(RED, LOW);
    digitalWrite(GREEN, LOW); 
  }
 delay(50); 
}
/**
        Author: TechPonder
        Date: August 18, 2015
        Description: Flame sensor is interfaced to arduino to detect Flame. Led and buzzer are interfaced to arduino to indicate the flame.
**/
void FLAME(){
  sensorValue = analogRead(sensorPin);
  if (sensorValue < 500)
  {
    digitalWrite(FAN, LOW);
    digitalWrite(BLUE, LOW);
    digitalWrite(RED, HIGH);
    digitalWrite(GREEN, LOW);
    tone(SOUND, 100);
    delay(500);
    noTone(SOUND);
    lcd.print("FIRE ALERT!");
    
    /**
        Author: Last Minute Engineers 
        Date: N.D
        Description: This code will initialize text message to the members of the family whenever it detects hazard or unwanted guest
**/
    mySerial.println("AT");
    updateSerial();
    mySerial.println("AT+CMGF=1");
    updateSerial();
    mySerial.println("AT+CMGS=\"+639555490096\"");
    updateSerial();
    mySerial.print("Anak, may sunog sa bahay umuwi ka agad bilis!");
    updateSerial();
    mySerial.write(26);
    
    
    delay(3000);
    lcd.clear();
    digitalWrite(BLUE, HIGH);
    digitalWrite(RED, LOW);
    digitalWrite(GREEN, LOW); 
  }
  delay(50);
}
/**
        Author: Last Minute Engineers 
        Date: N.D
        Description: This code will initialize text message to the members of the family whenever it detects hazard or unwanted guest
**/
void updateSerial()
{
  delay(500);
  while (Serial.available()) 
  {
    mySerial.write(Serial.read());//Forward what Serial received to Software Serial Port
  }
  while(mySerial.available()) 
  {
    Serial.write(mySerial.read());//Forward what Software Serial received to Serial Port
  }
}
