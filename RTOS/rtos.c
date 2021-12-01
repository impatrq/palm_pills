#include <16F887.h>
#fuses NOMCLR, NOBROWNOUT, NOLVP, INTRC_IO
#use delay(clock = 8MHz)
#use fast_io(B)
#use rtos(timer = 0, minor_cycle = 25ms)
   
#task(rate = 250ms, max = 25ms)                  // 1st RTOS task (executed every 250ms)
void funcion_alarma(){
  output_high(PIN_B2);
  output_high(PIN_B1);
  delay_ms(1000);
  output_low(PIN_B2);
  output_low(PIN_B1);
}
void main(){

    output_b(0);
    set_tris_b(0);
    rtos_run();
}