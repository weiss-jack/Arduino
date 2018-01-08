// essai de modif
// librairie d'affichage
#ifndef USE_DISPLAY
  #define USE_DISPLAY
  #define DISPLAY_Adafruit
#endif

//#include <SSD1306_OLED_TXT.h>
//SSD1306_OLED_TXT display(32);

#include <Adafruit_SSD1306.h>
Adafruit_SSD1306 display(3);
 
#if (SSD1306_LCDHEIGHT != 32)
  #error(F("Height incorrect, please fix Adafruit_SSD1306.h!"));
#endif

void initDisplay(){
  // SSD1306 initialization : by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x32)
  // init done
  
  // Show image buffer on the display hardware.
  // Since the buffer is intialized with an Adafruit splashscreen internally, this will display the splashscreen.
  display.display();
  display.setTextSize(1);
  display.setTextColor(WHITE);
} 
void clearScreen(){
  display.clearDisplay();
}
void affMsg(uint8_t x,uint8_t y,char *Msg,uint8_t Size){
  display.setTextSize(Size);
  display.setCursor(x*6,y*8);
  display.print(Msg);
}
void refreshScreen(){
   display.display();
}


