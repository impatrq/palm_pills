#include <16F887.h>
#fuses NOMCLR, NOBROWNOUT, NOLVP, INTRC_IO
#use delay(clock = 8MHz)
#use fast_io(B)
#use rtos(timer = 0, minor_cycle = 25ms)
   
#task(rate = 250ms, max = 25ms)                  // 1st RTOS task (executed every 250ms)
void funcion_alarma_1(){
  output_high(PIN_B2);
  output_high(PIN_B1);
  delay_ms(1000);
  output_low(PIN_B2);
  output_low(PIN_B1);
}
#task(rate = 500ms, max = 25ms)                  // 1st RTOS task (executed every 250ms)
void funcion_alarma_2(){
  output_high(PIN_B4);
  output_high(PIN_B3);
  delay_ms(1000);
  output_low(PIN_B4);
  output_low(PIN_B3);
}
void main(){
    output_b(0);
    set_tris_b(0b00101001);
    rtos_run();
}