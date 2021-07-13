/*Se definen los puertos del display*/
#define LCD_RS_PIN      PIN_D0
#define LCD_RW_PIN      PIN_D1
#define LCD_ENABLE_PIN  PIN_D2
#define LCD_DATA4       PIN_D3
#define LCD_DATA5       PIN_D4
#define LCD_DATA6       PIN_D5
#define LCD_DATA7       PIN_D6
//End LCD module connections

#include <16F877a.h>                      //incluimos el pic que vamos a usar
#fuses HS,NOWDT,NOPROTECT,NOLVP,PUT                       
#use delay(clock = 8MHz)
#use fast_io(B)                           //Establezco que voy a usar el puerto B (es la directiva)
#use fast_io(D)                           //Establezco que voy a usar el puerto C (es la directiva)
#use rs232(uart1, baud = 9600)
#include <lcd.c>                          //incluyo el .c del lcd
#use I2C(master, I2C1, FAST = 100000)
#include "modulo_de_voz.c"                //incluyo el .c del modulo de voz
int x=0;                                  //declaracion de una variable
int z=0;                                  //declaracion de una variable
int1 alarm1_status, alarm2_status;        //declaracion de variables
char time[]     = "  :  :  ",             // declaracion de alarma tipo char (char representa caracteres individuales)
     calendar[] = "      /  /20  ",       // declaracion de alarma tipo char (char representa caracteres individuales) 
     alarm1[]   = "A1:   :  :00", alarm2[]   = "A2:   :  :00", // declaracion de alarma tipo char (char representa caracteres individuales)
     temperature[] = "T:   .   C"; // declaracion de alarma tipo char (char representa caracteres individuales)
int8  i, second, minute, hour, day, date, month, year, // declaracion de variables de 8 byts
      alarm1_minute, alarm1_hour, alarm2_minute, alarm2_hour, // declaracion de variables de 8 byts
      status_reg;                                      // declaracion de variables de 8 byts
#INT_EXT                                         // External interrupt routine // interrupcion interna routine
void ext_isr(void){
  clear_interrupt(INT_EXT);
}
/*Función de lectura de datos de tiempo y calendario*/
void DS3231_read(){                              //Función de lectura de datos de tiempo y calendario
  i2c_start();                                   // empieza el i2c
  i2c_write(0xD0);                               // Direccion del DS3231
  i2c_write(0);                                  //se envia la direccion de registro (registro de segundos)
  i2c_start();                                   //inicia i2c
  i2c_write(0xD1);                               // inicia la lectura de datos
  second = i2c_read(1);                          // Lee los segundos del registro 0
  minute = i2c_read(1);                          // Lee los minutos del registro 1
  hour   = i2c_read(1);                          // Lee la hora del registro 2
  day    = i2c_read(1);                          // Lee el dia del registro 3
  date   = i2c_read(1);                          // Lee la fecha del registro 4
  month  = i2c_read(1);                          // Lee el mes del registro 5
  year   = i2c_read(0);                          //Lee el año del registro 6
  i2c_stop();                                    //Detiene el i2c 
}
/*Llama a la funcion lectura de alarmas*/
void alarms_read_display(){                      // Read and display alarm1 and alarm2 data function // Leer y mostrar la función de datos de alarma1 y alarma2
  int8 control_reg, temperature_lsb;             // no se usa
  signed int8 temperature_msb;                   // no se usa
  i2c_start();                                   //inicia el protocolo I2C
  i2c_write(0xD0);                               // direccion del DS3231
  i2c_write(0x08);                               // se envia la direccion de registro (registro de los minutos alarm1)
  i2c_start();                                   //inicia i2c
  i2c_write(0xD1);                               // inicia la lectura de datos
  alarm1_minute = i2c_read(1);                   //lectura de los minutos de alarma1
  alarm1_hour   = i2c_read(1);                   //lectura de las horas de alarma1
  i2c_read(1);                                   //salta el dia de alarm1
  alarm2_minute = i2c_read(1);                   //lectura de los minutos de alarma2
  alarm2_hour   = i2c_read(1);                   //lectura de las horas de alarma1
  i2c_read(1);                                   //salta el dia de alarm2
  control_reg = i2c_read(1);                     //lee el control de registro de DS3231
  status_reg  = i2c_read(1);                     //Lee las estaditicas del registro DS3231
  i2c_read(1);                 // Lee la temperatura MSB (no se usa)
  i2c_read(0);                 //Lee la temperatura LSB (no se usa)
  i2c_stop();                  //Detencion del i2c
  //Convierte de BCD a Decimal
  alarm1_minute = (alarm1_minute >> 4) * 10 + (alarm1_minute & 0x0F);
  alarm1_hour   = (alarm1_hour   >> 4) * 10 + (alarm1_hour & 0x0F);
  alarm2_minute = (alarm2_minute >> 4) * 10 + (alarm2_minute & 0x0F);
  alarm2_hour   = (alarm2_hour   >> 4) * 10 + (alarm2_hour & 0x0F);
  //Finaliza la conversion
  alarm1[8]     = alarm1_minute % 10  + 48;  
  alarm1[7]     = alarm1_minute / 10  + 48;
  alarm1[5]     = alarm1_hour   % 10  + 48;
  alarm1[4]     = alarm1_hour   / 10  + 48;
  alarm2[8]     = alarm2_minute % 10  + 48;
  alarm2[7]     = alarm2_minute / 10  + 48;
  alarm2[5]     = alarm2_hour   % 10  + 48;
  alarm2[4]     = alarm2_hour   / 10  + 48;
  alarm1_status = bit_test(control_reg, 0);      //Leer el bit de habilitación de interrupción de alarma1 (A1IE) del registro de control DS3231
  alarm2_status = bit_test(control_reg, 1);      //Leer el bit de habilitación de interrupción de alarma2 (A2IE) del registro de control DS3231                        
  lcd_gotoxy(21, 1);                             //Ir a la columna 1 fila 3
  printf(lcd_putc, alarm1);                      //Mostrar alarma 1
  lcd_gotoxy(38, 1);                             //Va a la columna 18, fila 3
  if(alarm1_status)  lcd_putc("ON ");            //Si A1IE es igual a uno muestra "on"
  else               lcd_putc("OFF");            //Si A1IE es igual a cero muestra "on"
  lcd_gotoxy(21, 2);                             //Va a la fila 4 de la columna 1
  printf(lcd_putc, alarm2);                      // Mostrar alarma2
  lcd_gotoxy(38, 2);                             // Va a la columna 18, fila 4
  if(alarm2_status)  lcd_putc("ON ");            //Si A2IE es igual a uno muestra "on"
  else               lcd_putc("OFF");            //Si A2IE es igual a cero muestra "on"
  
}
void calendar_display(){                        //LLama función de calendario
  switch(day){
    case 1:  strcpy(calendar, "Sun   /  /20  "); break;
    case 2:  strcpy(calendar, "Mon   /  /20  "); break;
    case 3:  strcpy(calendar, "Tue   /  /20  "); break;
    case 4:  strcpy(calendar, "Wed   /  /20  "); break;
    case 5:  strcpy(calendar, "Thu   /  /20  "); break;
    case 6:  strcpy(calendar, "Fri   /  /20  "); break;
    case 7:  strcpy(calendar, "Sat   /  /20  "); break;
    default: strcpy(calendar, "Sat   /  /20  "); break;
  }
  calendar[13] = year  % 10 + 48;
  calendar[12] = year  / 10 + 48;
  calendar[8]  = month % 10 + 48;
  calendar[7]  = month / 10 + 48;
  calendar[5]  = date  % 10 + 48;
  calendar[4]  = date  / 10 + 48;
  lcd_gotoxy(1, 2);                              //va a la columna 1 fila 2
  printf(lcd_putc, calendar);                    // Mostrar calendario
}
/*Llama a funcion DS3231*/
void DS3231_display(){ 
//convierte de BCD a decimal
  second = (second >> 4) * 10 + (second & 0x0F);
  minute = (minute >> 4) * 10 + (minute & 0x0F);
  hour = (hour >> 4) * 10 + (hour & 0x0F);
  date = (date >> 4) * 10 + (date & 0x0F);
  month = (month >> 4) * 10 + (month & 0x0F);
  year = (year >> 4) * 10 + (year & 0x0F);
  // termina la conversion
  time[7]     = second % 10  + 48;
  time[6]     = second / 10  + 48;
  time[4]     = minute % 10  + 48;
  time[3]     = minute / 10  + 48;
  time[1]     = hour   % 10  + 48;
  time[0]     = hour   / 10  + 48;
  calendar_display();                            // Llamar a la función de visualización del calendario
  lcd_gotoxy(1, 1);                              // Ir a la columna 1 fila 1
  printf(lcd_putc, time);                        //Mostrar la variable time
}
void blink(){
  int8 j = 0;
  while(j < 10 && (input(PIN_B3) || i >= 5) && input(PIN_B2) && (input(PIN_B1) || i < 5)){
    j++;
    delay_ms(25);
  }
}
int8 edit(parameter, x, y){                      //No entiendo que hace aca
  while(!input(PIN_B3) || !input(PIN_B1));       //Espera hasta que se suelte el botón RB0
  while(TRUE){
    while(!input(PIN_B2)){                       //Si se presiona el botón RB2
      parameter++;
      if(((i == 0) || (i == 5)) && parameter > 23)    //Si horas> 23 ==> horas = 0
        parameter = 0;
      if(((i == 1) || (i == 6)) && parameter > 59)    //Si minutos> 59 ==> minutos = 0
        parameter = 0;
      if(i == 2 && parameter > 31)               // Si fecha> 31 ==> fecha = 1
        parameter = 1;
      if(i == 3 && parameter > 12)               //Si mes> 12 ==> mes = 1
        parameter = 1;
      if(i == 4 && parameter > 99)               // Si año> 99 ==> año = 0
        parameter = 0;
      if(i == 7 && parameter > 1)                // Para alarmas ON o OFF (1: alarma ON, 0: alarma OFF)
        parameter = 0;
      lcd_gotoxy(x, y);
      if(i == 7){                                // For alarms ON & OFF // Para alarmas ON y OFF
        if(parameter == 1)  lcd_putc("ON ");
        else                lcd_putc("OFF");
      }
      else
        printf(lcd_putc,"%02u", parameter);      //Se visualiza los parametros
      if(i >= 5){
        DS3231_read();                           //Leer datos de DS3231
        DS3231_display();                        //Muestra la hora y el calendario de DS3231
      }
      delay_ms(200);                             //Espera 200ms
    }
    lcd_gotoxy(x, y);                            //ir a la columna x y la fila y de la pantalla LCD
    lcd_putc("  ");                              //imprime dos espacios
    if(i == 7) lcd_putc(" ");                    //Espacio de impresión (para activar y desactivar alarmas)
    blink();                                     //llamar a la funcion blink
    lcd_gotoxy(x, y);                            //Vaya a la columna x de la pantalla LCD y a la fila y
    if(i == 7){                                  // For alarms ON & OFF // Para alarmas ON y OFF
      if(parameter == 1)  lcd_putc("ON ");
      else                lcd_putc("OFF");
    }
    else
      printf(lcd_putc,"%02u", parameter);        //mostrar los parametros
    blink();
    if(i >= 5){
      DS3231_read();
      DS3231_display();}
    if((!input(PIN_B1) && i < 5) || (!input(PIN_B3) && i >= 5)){
      i++;                                       //Incrementar 'i' para el siguiente parámetro
      return parameter;                          //Devuelve el valor del parámetro y sale
    }
  }
}
int A = 0;
void envio_datos(){
      i2c_start();                               //Inicia el i2c
      i2c_write(0xD0);                           //direccion del DS3231
      i2c_write(0);                              //Envia dirección de registro (dirección de segundos)
      i2c_write(0);                              // Restablecer los segundos e iniciar el oscilador
      i2c_write(minute);                         // Escribe el valor de los minutos en DS3231
      i2c_write(hour);                           // Escribe el valor de la hora en DS3231
      i2c_write(day);                            // Escribe el valor del día
      i2c_write(date);                           // Escribe el valor de la fecha en DS3231
      i2c_write(month);                          // Escribe el valor del mes en DS3231
      i2c_write(year);                           /// Escribe el valor del año en DS3231
      i2c_stop();                                // Detener I2C
} 
void envio_datos_alarma(){
      i2c_start();                               //Empieza el i2c
      i2c_write(0xD0);                           // Dirección DS3231
      i2c_write(7);                              // Enviar dirección de registro (alarma1 segundos)
      i2c_write(0);                              // Escribe 0 en alarm1 segundos
      i2c_write(alarm1_minute);                  // Escribe el valor de los minutos de alarma1 en DS3231
      i2c_write(alarm1_hour);                    // Escribe el valor de las horas de alarma1 en DS3231
      i2c_write(0x80);                           // Alarma1 cuando coinciden las horas, los minutos y los segundos
      i2c_write(alarm2_minute);                  // Escribe el valor de la minutos de alarma2 en DS3231
      i2c_write(alarm2_hour);                    //Escribe el valor dde hora de alarma 2 en DS3231
      i2c_write(0x80);                           // Alarma2 cuando coinciden las horas y los minutos
      i2c_write(4 | alarm1_status | (alarm2_status << 1)); // Escribe datos en el registro de control DS3231 (habilita la interrupción en caso de alarma)
      i2c_write(0);                              // Borrar los bits de la bandera de alarma
      i2c_stop();                                // Detener I2C
      delay_ms(200);//esperar 200ms
      blink (); //(Si o si agregas esto)
}
void funcion_gabinete_1(){
          A++;
          lcd_gotoxy(2, 16);
            delay_ms(750);
             if (A > 5)
             {
                A = 0;
             }
}
void funcion_gabinete_2(){
            A--;
            lcd_gotoxy(2, 16);
            delay_ms(750);

            if (A == 0)
            {
                A = 5;
            }
}
void control_de_alarmas_activas(){
output_low(PIN_B4);                        // Apaga el indicador de alarma
      i2c_start();                               // Iniciar I2C
      i2c_write(0xD0);                           // Dirección DS3231
      i2c_write(0x0E);                           // Enviar dirección de registro (registro de control)
      // Escribir datos en el registro de control (apagar la alarma ocurrida y mantener la otra como está)
      i2c_write(4 | (!bit_test(status_reg, 0) & alarm1_status) | ((!bit_test(status_reg, 1) & alarm2_status) << 1));
      i2c_write(0);                              // Borrar los bits de la bandera de alarma
      i2c_stop();                                // Detener I2C
}
void main(){
//Configuracion de puertos 
  output_b(0);
  set_tris_b(0x0F);                              //Configurar RB0 ~ 3 como pines de entrada
  set_tris_d(0);                                 //Configurar todos los pines de PORTS como salidas
  port_b_pullups(TRUE);                          //Habilitar pull-ups internos PORT

//Habilitar las interrupciones
  enable_interrupts(GLOBAL);                     //Habilitar interrupciones globales
  enable_interrupts(INT_EXT);                    // Habilita la interrupción externa con borde de mayor a menor

//Habilito el LCD
  lcd_init();                                    //Inicia el modulo LCD
  lcd_putc('\f');                                //Limpio el LCD
  
  reproduccion_pista(13, 1);
  while(TRUE){
    if(!input(PIN_B1)){                          //Si se presiona el botón RB1
      i = 0;
      hour   = edit(hour, 1, 1);
      minute = edit(minute, 4, 1);
      while(!input(PIN_B1));                     //Espere hasta que se suelte el botón RB0
      while(TRUE){
        while(!input(PIN_B1)){                   // Si se presiona el botón RB2
          day++;                                 // Día de incremento
          if(day > 7) day = 1;
          //Muestra
          calendar_display();                    //Llama a mostraar el calendario
        }
        lcd_gotoxy(1, 2);                        // Ir a la columna 1 fila 2
        lcd_putc("   ");                         // Imprime 3 espacios
        blink();
        lcd_gotoxy(1, 2);                        // Ir a la columna 1 fila 2
        printf(lcd_putc, calendar);              // Imprime calendario
        blink();                                 //Llama a la funcion blink         
          break;
      }
      date = edit(date, 5, 2);                   //editar fecha
      month = edit(month, 8, 2);                 //editar mes
      year = edit(year, 13, 2);                  //editar año
      //convierte de BCD a decimal
      minute = ((minute / 10) << 4) + (minute % 10);
      hour = ((hour / 10) << 4) + (hour % 10);
      date = ((date / 10) << 4) + (date % 10);
      month = ((month / 10) << 4) + (month % 10);
      year = ((year / 10) << 4) + (year % 10);
      //finaliza la conversion
      //Escribir datos de tiempo y calendario en DS3231 RTC
      //Bloque por bloque
      envio_datos();
    }
    if(!input(PIN_B3)){                          // Si se presiona el botón RB3
      while(!input(PIN_B3));                     // Espere hasta que se suelte el botón RB3
      i = 5;
      alarm1_hour   = edit(alarm1_hour, 25, 1);
      alarm1_minute = edit(alarm1_minute, 28, 1);
      alarm1_status = edit(alarm1_status, 38, 1);
      i = 5;
      alarm2_hour   = edit(alarm2_hour, 25, 2);
      alarm2_minute = edit(alarm2_minute, 28, 2);
      alarm2_status = edit(alarm2_status, 38, 2);
      alarm1_minute = ((alarm1_minute / 10) << 4) + (alarm1_minute % 10);
      alarm1_hour   = ((alarm1_hour / 10) << 4) + (alarm1_hour % 10);
      alarm2_minute = ((alarm2_minute / 10) << 4) + (alarm2_minute % 10);
      alarm2_hour   = ((alarm2_hour / 10) << 4) + (alarm2_hour % 10);
      // Write alarms data to DS3231 //Escribir datos de alarmas en DS3231
    envio_datos_alarma();
    }
    if(!input(PIN_B2) && input(PIN_B4)){         // Cuando se presiona el botón B2 con alarma (Restablecer y apagar la alarma)
    control_de_alarmas_activas();
    }
    DS3231_read();                               // Leer los parámetros de tiempo y calendario de DS3231 RTC
    alarms_read_display();                       // Leer y mostrar los parámetros de las alarmas
    DS3231_display();                            // Mostrar hora y calendario
    /*Apartir de aca es lo del gabinete, que lo muestra en el display y podes subir y bajar los gabinetes*/
    lcd_putc("Gab:");
    printf(lcd_putc, "%d", A);
    lcd_gotoxy(2, 16);
        if (input(pin_A0) == 0)
        {
          funcion_gabinete_1();
             }
        if (input(pin_A1) == 0)
        {
          funcion_gabinete_2();
        }
    delay_ms(50);                                // Wait 50ms // Espera 50ms
  }
}
