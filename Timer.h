

volatile int interruptCounter;

#if defined(ESP_PLATFORM) 
  hw_timer_t * timer = NULL;
  portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;
#else
  #include "TimerOne.h"
#endif
/************************************/ 
void onTimer() {
long rawCurrent, rawVoltage;
#if defined(ESP_PLATFORM)
  portENTER_CRITICAL_ISR(&timerMux);
  interruptCounter++;
  portEXIT_CRITICAL_ISR(&timerMux);
#else
  interruptCounter++;
#endif

  rawCurrent =(analogRead(ADC_Current_PIN)*ADC_Gain - 2.5)*Current_Gain; // offset voltage is 2.5v
  rawVoltage = analogRead(ADC_Voltage_PIN)*ADC_Gain*VoltageDivisor;
  Current = (rawCurrent * (1. - filterCoef)) + (Current  *  filterCoef);
  Voltage = (rawVoltage * (1. - filterCoef)) + (Voltage  *  filterCoef);

  Current = 100.;
  Voltage = 50.; 
}

void initTimer(int duration){
#if defined(ESP_PLATFORM)
  // Clock freq. = 80 MHz ; Divider = 80MHz*500µs = 40000 ==> tick at 2 kHz
  timer = timerBegin(0, 80*duration, true); // timer_id = 0; divider=40000; countUp = true;
  timerAttachInterrupt(timer, &onTimer, true); // edge = true
  timerAlarmWrite(timer, TIMER_DIZIEME, true); //100 ms
  timerAlarmEnable(timer);
#else
  Timer1.initialize(duration); // set a timer of length 500 microseconds ( or 2kHz)
  Timer1.attachInterrupt( onTimer ); // attach the service routine here
  
#endif
}


  
