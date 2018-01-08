/*
  Energy on SD card datalogger

 The circuit:
 * Voltage on analog input A0 through a resistor ladder 
 * Current on analog input A1 through ACS712 current sensor
 * Display on 128*32 OLED graphic Display SSD1306 via I2C
 * Debug output via serial link
 * SD card attached to SPI bus 

 created  18 dec 2017
 by J. WEISS

 */

#define TIMER_PERIOD   500 // 500 microseconds (2 kHz)
#define TIMER_DIZIEME  1*100000/TIMER_PERIOD // for a tick per 1/10 second 
#define TIMER_SECOND  1*1000000/TIMER_PERIOD // for a tick per second 

#if defined(ESP_PLATFORM)
  #define ADC_Voltage_PIN 32
  #define ADC_Current_PIN 33
  #define ADC_Resolution 4096 // 12-bit ADC
  #define ADC_Range 3.2 // 0 - 3.2V range (11 dB attenuation)
#elif defined(ARDUINO)
  #define ADC_Voltage_PIN 3
  #define ADC_Current_PIN 4
  #define ADC_Resolution 1024 // 10-bit ADC
  #define ADC_Range 5 // 0 - 5V range
#endif

const float ADC_Gain = ADC_Range / ADC_Resolution ;// (VoltageReference / (2^^12)); 0 - 3.2V range with 12 bits ADC (ESP32)
const float VoltageDivisor = 1.; //R2 / (R1 + R2)
const float Current_Gain = 1000. / 66.0; // ACS712-30A : 66 mV/A
const float filterCoef=0.1; // smoothing of sensor values

float Voltage;
float Current;
float Watts;
float TotalAmperes;
float AmpereHour;
float Energy;
byte  Dizaine;
char Message[32];
char Message2[8];
volatile long elapsedTime;
volatile long currentTop;
volatile long previousTop;

 
//#include "Affichage.h"
#include "Affichage_u8x8.h"
#include "Carte_SD.h"
#include "Timer.h"


#define DEBUG





float smoothedCurrent;


void setup()   {
  Serial.begin(115200);
#ifdef DEBUG
  Serial.println("Démarrage");
  #if defined(ESP_PLATFORM)
    Serial.print("ESP_PLATFORM : ");Serial.println(ESP_PLATFORM);
  #elif defined(ARDUINO)
    Serial.print("ARDUINO : ");Serial.println(ARDUINO);
  #elif defined(ARDUINO_ARCH_ESP32)
    Serial.print("ARDUINO_ARCH_ESP32 : ");Serial.println(ARDUINO_ARCH_ESP32);    
  #endif
#endif

  initTimer(TIMER_PERIOD); // tick at 500 microseconds

#ifdef USE_SD
  initSDCard();
  writeHeader(); // Write data header.
#endif
  
#ifdef USE_DISPLAY
    initDisplay();
#endif

  delay(500);
  //analogSetPinAttenuation(A4, ADC_11db);
}


void loop() {
#if defined(ESP_PLATFORM) 
  if (interruptCounter > 0) {
    portENTER_CRITICAL(&timerMux);
    interruptCounter--;
    portEXIT_CRITICAL(&timerMux);
#else
  //Serial.println(interruptCounter);
  if (interruptCounter > TIMER_DIZIEME) {
    interruptCounter = 0;

#endif
  if (Dizaine < 9) {
      Dizaine++;
  }
  else {
    Dizaine = 0;
    AmpereHour += Current/36.;
    Watts = Voltage*Current; // power=voltage*current
      //AmpereHour = (TotalAmperes/elapsedTime)/3600;
    Energy += Watts/36000.; 
      // energy=(watt*time)/(1000*3600); for reading in kWh

      elapsedTime++;
/*      currentTop = millis();
      Serial.print("Top : "); Serial.println(currentTop - previousTop);
      previousTop = currentTop; 
*/  

#ifdef DEBUG
      Serial.print(F("Temps : ")); Serial.print(elapsedTime); Serial.print(F(" s "));
      Serial.print(F("Voltage : ")); Serial.print(Voltage,1); Serial.print(F(" V ")); Serial.print(F("CURRENT :")); Serial.print(Current,1); Serial.print(F(" A "));
      Serial.print(F("POWER :"));Serial.print(Watts);Serial.print(F(" Watt "));Serial.print(F("Energy :"));Serial.print(Energy);Serial.print(F(" Watt-Hour"));
      Serial.print(F("Energy :"));Serial.print(AmpereHour,0);Serial.print(F(" mAh "));
      Serial.println("");
#endif
#ifdef USE_DISPLAY
      clearScreen();
      dtostrf(Voltage,4,1,Message); strcat(Message, " V ");
      dtostrf(Watts,5,0,Message2); strcat(Message, Message2); strcat(Message, " W ");
      affMsg(0,0,Message,1);
      
      int Amp = int(Current);
      int Centis = int (abs(100*(Current-Amp)));
      sprintf (Message, "%3d.%02d", Amp,Centis);
      affMsg(0,1,Message,2);
      affMsg(13,2,"A",1);
       
      Message[0] = (char)0;
      if (Energy > 100){
        dtostrf(Energy,4,0,Message); 
      }
      else {
        dtostrf(Energy,3,1,Message);  
      }
      strcat(Message, " Wh");
      
      if (AmpereHour > 1000){
        dtostrf(AmpereHour/1000.,3,1,Message2); 
        strcat(Message, Message2); strcat(Message, " Ah");
      }
      else {
        dtostrf(AmpereHour,5,0,Message2); 
        strcat(Message, Message2); strcat(Message, " mAh");
      }
      affMsg(0,3,Message,1);
      refreshScreen();
/*
      affMsg(0,0,"Energy Meter",1);
      int Amp = int(Current);
      int Centis = int (abs(100*(Current-Amp)));
      sprintf (Message, "%3d.%02d", Amp,Centis);
      affMsg(0,1,Message,2);
      affMsg(13,2,"A",1); 
      Message[0] = (char)0;
      dtostrf(Voltage,4,1,Message); strcat(Message, " V ");
      dtostrf(Watts,5,0,Message2); strcat(Message, Message2); strcat(Message, " W ");
      affMsg(0,3,Message,1);
      refreshScreen();
      */
#endif
   
#ifdef USE_SD
      // make a string for assembling the data to log:
      String dataString = String(elapsedTime);dataString += ";";
      dataString += String(Current);dataString += ";";
      dataString += String(Voltage);dataString += ";";
      dataString += String(Watts);dataString += ";";
      dataString += String(Energy);dataString += ";\n";
      SD_Record(&dataString);
#endif;
    }
  }  
}

