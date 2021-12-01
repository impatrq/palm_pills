#include <16F887.h>
#fuses NOMCLR, NOBROWNOUT, NOLVP, INTRC_IO
#use delay(clock = 8MHz)
#use fast_io(B)
#use rtos(timer = 0, minor_cycle = 50ms)


void main(){

    output_b(0);
    set_tris_b(0);
    rtos_run();
}