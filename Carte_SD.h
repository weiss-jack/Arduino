/*
 * SD card attached to SPI bus as follows:
 ** MOSI - pin 11
 ** MISO - pin 12
 ** CLK - pin 13
 ** CS D pin 4 (for MKRZero SD: SDCARD_SS_PIN)
 
 */

#ifndef USE_SD
  #define USE_SD
#endif

#include <SD.h>
const uint8_t chipSelect = 4;
#define FILE_BASE_NAME "Data" 
const uint8_t BASE_NAME_SIZE = sizeof(FILE_BASE_NAME) - 1;
char fileName[13] = FILE_BASE_NAME "00.csv"; 
bool SDCard_OK = false;

void initSDCard(){
  Serial.print(F("Initializing SD card..."));

  if (!SD.begin(chipSelect)){
    Serial.println(F("Card failed, or not present"));
    SDCard_OK = false;
  }
  else { //    SD Card present;
    Serial.println(F("card initialized."));
    delay(200);
    // Find an unused file name.
//   if (BASE_NAME_SIZE > 6) error("FILE_BASE_NAME too long");
 
   while (SD.exists(fileName)) {
     if (fileName[BASE_NAME_SIZE + 1] != '9') {
       fileName[BASE_NAME_SIZE + 1]++;
       SDCard_OK = true;
     } 
     else if (fileName[BASE_NAME_SIZE] != '9') {
       fileName[BASE_NAME_SIZE + 1] = '0';
       fileName[BASE_NAME_SIZE]++;
       SDCard_OK = true;
     } 
     else {  
      SDCard_OK = false;
      Serial.println("Can't create file name on SD Card");
     }
    
   }
   Serial.print(" Fichier : ");Serial.println(fileName);
  }
}

//==============================================================================
// User functions.  Edit writeHeader() and logData() for your requirements.

//------------------------------------------------------------------------------



//------------------------------------------------------------------------------
void SD_Record(String *Message) {
  if (SDCard_OK) {
   File dataFile = SD.open(fileName, FILE_WRITE);
   // if the file is available, write to it:
   if (dataFile) {
      Serial.print("SD : ");Serial.println(*Message);
      dataFile.println(*Message);
      //dataFile.println("Coucou");
      dataFile.close();
      // print to the serial port too:
      //Serial.println(dataString);
   }
   // if the file isn't open, pop up an error:
   else {
      Serial.println(F("error opening file"));
      SDCard_OK =false;
   }
  }
} 
// Write data header.
void writeHeader() {
  String Msg = F("Seconds, Current, Voltage, Power, Energy");
  SD_Record(&Msg);

  //SD_Record(&String(F("Seconds, Current, Voltage, Power, Energy")));
}
 
