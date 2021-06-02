#include <16f877A.h>                            //Utilizamos la libreía del PIC 18F4550
#fuses HS,NOWDT,NOPUT,NOPROTECT                 //Configuración de fusibles.
#use delay (clock=8M)                           //Frecuencia de Oscilador 8MHz.
#BYTE port_b = 0xF81                            //Dirección de Puerto B en memoria.

int x=0;                                        //Variable x tipo entero, para el contador.

void diez_pulsaciones()
{
    for (x = 1; x <= 10; x++)
    {
        output_high(PIN_B5);
        delay_ms(10);
        output_low(PIN_B5);
        delay_ms(60);
    }
}

#int_ext             //Habilitamos la librería de Interrrupción Externa
Interrupcion_Pulso() //Asignamos el nombre de "Interrupcion_Pulso"
{
    ext_int_edge(H_TO_L); //Activa la interrupción en RB0 por flanco de bajada.
    diez_pulsaciones();   // Llamar a la funcion diez_pulsaciones
}

void main (void)                                //Funcion principal main
{
   set_tris_b(0b00000001);                      //Establecemos el pin RB0 como entrada.
   enable_interrupts(GLOBAL);                   //Habilitamos todas las interrupciones.
   enable_interrupts(INT_EXT);                  //Habilitamos especificamente la interrupción externa RB0.

}