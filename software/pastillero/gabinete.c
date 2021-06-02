#include <16f877a.h>
#use delay(clock = 4000000)
#fuses xt, nowdt, noprotect
#include <lcd.c>

int A = 0;
void main(void)

{
    lcd_init();
    lcd_putc("Gabinete");

    while (true)
    {
        lcd_gotoxy(1, 2);
        printf(lcd_putc, "%d", A);
        if (input(pin_A0) == 0)
        {

            A++;
            delay_ms(250);

            if (A > 5)
            {
                A = 0;
            }
        }
        if (input(pin_A1) == 0)
        {
            A--;
            delay_ms(250);

            if (A == 0)
            {
                A = 5;
            }
        }
    }
}