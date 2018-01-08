#ifndef USE_DISPLAY
  #define USE_DISPLAY
  #define DISPLAY_u8x8
#endif

#include <U8x8lib.h>
U8X8_SSD1306_128X32_UNIVISION_HW_I2C u8x8(/* reset=*/ U8X8_PIN_NONE, /* clock=*/ SCL, /* data=*/ SDA);   // pin remapping with ESP8266 HW I2C

void initDisplay(){
   u8x8.begin();
   u8x8.setPowerSave(0);
   //u8x8.setFont(u8x8_font_chroma48medium8_r);
   u8x8.setFont(u8x8_font_pcsenior_f);
   u8x8.setCursor(0,0);
}

void clearScreen(){
   //u8x8.clearDisplay();
}

void affMsg(uint8_t x,uint8_t y,char *Msg,uint8_t Size){
   if (Size == 1) u8x8.drawString(x, y, Msg);
   if (Size == 2) u8x8.draw2x2String(x, y, Msg);
}

void refreshScreen(){
}

