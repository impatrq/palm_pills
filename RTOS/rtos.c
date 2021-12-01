#include <16F887.h>
#fuses NOMCLR, NOBROWNOUT, NOLVP, INTRC_IO
#use delay(clock = 8MHz)
#use fast_io(B)
#use rtos(timer = 0, minor_cycle = 50ms)

#task(rate = 250ms, max = 50ms)                  // 1st RTOS task (executed every 250ms)
void led1(){
  output_high(PIN_B0);
}
void main(){

    output_b(0);
    set_tris_b(0);
    rtos_run();
}