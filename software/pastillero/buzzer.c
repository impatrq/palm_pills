#include <16f877A.h>                            //Utilizamos la libreía del PIC 18F4550
#fuses HS,NOWDT,NOPUT,NOPROTECT                 //Configuración de fusibles.
#use delay (clock=8M)                           //Frecuencia de Oscilador 8MHz.
#BYTE port_b = 0xF81                            //Dirección de Puerto B en memoria.
#BYTE port_d = 0xF83                            //Dirección de puerto D en memoria.

int x=0;                                        //Variable x tipo entero, para el contador.

#int_ext                                        //Habilitamos la librería de Interrrupción Externa  
Interrupcion_Pulso()                            //Asignamos el nombre de "Interrupcion_Pulso"                
                                                
{
   ext_int_edge(H_TO_L);                        //Activa la interrupción en RB0 por flanco de bajada.
   for (x=1; x<=10; x++)
   {

   output_high(PIN_D1);
   delay_ms(10);
   output_low(PIN_D1);
   delay_ms(60);
   }
}

void main (void)                                //Funcion principal main
{
   set_tris_b(0b00000001);                      //Establecemos el pin RB0 como entrada.
   set_tris_d(0b00000000);                      //Establecemos el Puerto D como salida.
   enable_interrupts(GLOBAL);                   //Habilitamos todas las interrupciones.
   enable_interrupts(INT_EXT);                  //Habilitamos especificamente la interrupción externa RB0.

}