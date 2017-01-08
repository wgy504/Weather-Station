#include "button_general.h"


void vButtonTask (void *pvParameters)
{
  int16_t sTimerBack = -1;
  
  while(1)
  {
    if(GET_BUT1) sTIMET1_CCR1_Val += 100;
    if(GET_BUT2) sTIMET1_CCR1_Val -= 100;
    
    if(sTIMET1_CCR1_Val > TIM3_PERIOD) sTIMET1_CCR1_Val = TIM3_PERIOD;
    if(sTIMET1_CCR1_Val < 100) sTIMET1_CCR1_Val = 100;
    
    if(sTimerBack != sTIMET1_CCR1_Val) {
      sTimerBack = sTIMET1_CCR1_Val;
      TIM_SetCompare1(TIM3, sTIMET1_CCR1_Val);
    }
    
    _delay_ms(250);
  }
}