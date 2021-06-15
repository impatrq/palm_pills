#define LCD_RS_PIN      PIN_D0
#define LCD_RW_PIN      PIN_D1
#define LCD_ENABLE_PIN  PIN_D2
#define LCD_DATA4       PIN_D3
#define LCD_DATA5       PIN_D4
#define LCD_DATA6       PIN_D5
#define LCD_DATA7       PIN_D6
//End LCD module connections

#include <16F877A.h>
#fuses HS,NOWDT,NOPROTECT,NOLVP,PUT                       
#use delay(clock = 8MHz)
#use fast_io(B)
#use fast_io(D)
#use rs232(uart1, baud = 9600)
#include <lcd.c>
#use I2C(master, I2C1, FAST = 100000)
#include "modulo_de_voz.c"
int1 alarm1_status, alarm2_status;
char time[]     = "  :  :  ",
     calendar[] = "      /  /20  ",
     alarm1[]   = "A1:   :  :00", alarm2[]   = "A2:   :  :00",
     temperature[] = "T:   .   C";
int8  i, second, minute, hour, day, date, month, year,
      alarm1_minute, alarm1_hour, alarm2_minute, alarm2_hour,
      status_reg;
#INT_EXT                                         // External interrupt routine // interrupcion interna routine
void ext_isr(void){
  output_high(PIN_B4);
  clear_interrupt(INT_EXT);
}
void DS3231_read(){                              // Read time & calendar data function  //Función de lectura de datos de tiempo y calendario
  i2c_start();                                   // Start I2C protocol  // empieza el i2c
  i2c_write(0xD0);                               // DS3231 address // direccion del DS3231
  i2c_write(0);                                  // Send register address (seconds register) //se envia la direccion de registro (registro de segundos)
  i2c_start();                                   // Restart I2C //inicia i2c
  i2c_write(0xD1);                               // Initialize data read // inicia la lectura de datos
  second = i2c_read(1);                          // Read seconds from register 0 // Lee los segundos del registro 0
  minute = i2c_read(1);                          // Read minutes from register 1 // Lee los minutos del registro 1
  hour   = i2c_read(1);                          // Read hour from register 2 // Lee la hora del registro 2
  day    = i2c_read(1);                          // Read day from register 3 // Lee el dia del registro 3
  date   = i2c_read(1);                          // Read date from register 4 // Lee la fecha del registro 4
  month  = i2c_read(1);                          // Read month from register 5 // Lee el mes del registro 5
  year   = i2c_read(0);                          // Read year from register 6 //Lee el año del registro 6
  i2c_stop();                                    // Stop I2C protocol //Detiene el i2c 
}
void alarms_read_display(){                      // Read and display alarm1 and alarm2 data function // Leer y mostrar la función de datos de alarma1 y alarma2
  int8 control_reg, temperature_lsb;
  signed int8 temperature_msb;
  i2c_start();                                   // Start I2C protocol //inicia el protocolo I2C
  i2c_write(0xD0);                               // DS3231 address // direccion del DS3231
  i2c_write(0x08);                               // Send register address (alarm1 minutes register) // se envia la direccion de registro (registro de los minutos alarm1)
  i2c_start();                                   // Restart I2C //inicia i2c
  i2c_write(0xD1);                               // Initialize data read // inicia la lectura de datos
  alarm1_minute = i2c_read(1);                   // Read alarm1 minutes //lectura de los minutos de alarma1
  alarm1_hour   = i2c_read(1);                   // Read alarm1 hours  //lectura de las horas de alarma1
  i2c_read(1);                                   // Skip alarm1 day/date register //salta el dia de alarm1
  alarm2_minute = i2c_read(1);                   // Read alarm2 minutes //lectura de los minutos de alarma2
  alarm2_hour   = i2c_read(1);                   // Read alarm2 hours //lectura de las horas de alarma1
  i2c_read(1);                                   // Skip alarm2 day/date register //salta el dia de alarm2
  control_reg = i2c_read(1);                     // Read the DS3231 control register //lee el control de registro de DS3231
  status_reg  = i2c_read(1);                     // Read the DS3231 status register //Lee las estaditicas del registro DS3231
  i2c_read(1);                 // Read temperature MSB // Lee la temperatura MSB
  i2c_read(0);                 // Read temperature LSB //Lee la temperatura LSB
  i2c_stop();                                    // Stop I2C protocol //Detencion del i2c
    // Convert BCD to decimal //convierte de BCD a Decimal
  alarm1_minute = (alarm1_minute >> 4) * 10 + (alarm1_minute & 0x0F);
  alarm1_hour   = (alarm1_hour   >> 4) * 10 + (alarm1_hour & 0x0F);
  alarm2_minute = (alarm2_minute >> 4) * 10 + (alarm2_minute & 0x0F);
  alarm2_hour   = (alarm2_hour   >> 4) * 10 + (alarm2_hour & 0x0F);
    // End conversion //finaliza la conversion
  alarm1[8]     = alarm1_minute % 10  + 48;
  alarm1[7]     = alarm1_minute / 10  + 48;
  alarm1[5]     = alarm1_hour   % 10  + 48;
  alarm1[4]     = alarm1_hour   / 10  + 48;
  alarm2[8]     = alarm2_minute % 10  + 48;
  alarm2[7]     = alarm2_minute / 10  + 48;
  alarm2[5]     = alarm2_hour   % 10  + 48;
  alarm2[4]     = alarm2_hour   / 10  + 48;
  alarm1_status = bit_test(control_reg, 0);      // Read alarm1 interrupt enable bit (A1IE) from DS3231 control register //Leer el bit de habilitación de interrupción de alarma1 (A1IE) del registro de control DS3231
  alarm2_status = bit_test(control_reg, 1);      // Read alarm2 interrupt enable bit (A2IE) from DS3231 control register //Leer el bit de habilitación de interrupción de alarma2 (A2IE) del registro de control DS3231                        
  lcd_gotoxy(21, 1);                             // Go to column 1 row 3 //Ir a la columna 1 fila 3
  printf(lcd_putc, alarm1);                      // Display alarm1 //Mostrar alarma 1
  lcd_gotoxy(38, 1);                             // Go to column 18 row 3 //Va a la columna 18, fila 3
  if(alarm1_status)  lcd_putc("ON ");            // If A1IE = 1 print 'ON' //Si A1IE es igual a uno muestra "on"
  else               lcd_putc("OFF");            // If A1IE = 0 print 'OFF' //Si A1IE es igual a cero muestra "on"
  lcd_gotoxy(21, 2);                             // Go to column 1 row 4 //Va a la fila 4 de la columna 1
  printf(lcd_putc, alarm2);                      // Display alarm2 // Mostrar alarma2
  lcd_gotoxy(38, 2);                             // Go to column 18 row 4 // Va a la columna 18, fila 4
  if(alarm2_status)  lcd_putc("ON ");            // If A2IE = 1 print 'ON' //Si A2IE es igual a uno muestra "on"
  else               lcd_putc("OFF");            // If A2IE = 0 print 'OFF' //Si A2IE es igual a cero muestra "on"
  
}
void calendar_display(){                         // Display calendar function //Mostrar función de calendario
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
  lcd_gotoxy(1, 2);                              // Go to column 1 row 2 //va a la columna 1 fila 2
  printf(lcd_putc, calendar);                    // Display calendar // Mostrar calendario
}
void DS3231_display(){
  // Convert BCD to decimal //convierte de BCD a decimal
  second = (second >> 4) * 10 + (second & 0x0F);
  minute = (minute >> 4) * 10 + (minute & 0x0F);
  hour = (hour >> 4) * 10 + (hour & 0x0F);
  date = (date >> 4) * 10 + (date & 0x0F);
  month = (month >> 4) * 10 + (month & 0x0F);
  year = (year >> 4) * 10 + (year & 0x0F);
  // End conversion // termiina la conversion
  time[7]     = second % 10  + 48;
  time[6]     = second / 10  + 48;
  time[4]     = minute % 10  + 48;
  time[3]     = minute / 10  + 48;
  time[1]     = hour   % 10  + 48;
  time[0]     = hour   / 10  + 48;
  calendar_display();                            // Call calendar display function // Llamar a la función de visualización del calendario
  lcd_gotoxy(1, 1);                              // Go to column 1 row 1 //// Ir a la columna 1 fila 1
  printf(lcd_putc, time);                        // Display time //Mostrar el tiempo
}
void blink(){
  int8 j = 0;
  while(j < 10 && (input(PIN_B3) || i >= 5) && input(PIN_B2) && (input(PIN_B1) || i < 5)){
    j++;
    delay_ms(25);
  }
}
int8 edit(parameter, x, y){
  while(!input(PIN_B3) || !input(PIN_B1));       // Wait until button RB0 is released //Espera hasta que se suelte el botón RB0
  while(TRUE){
    while(!input(PIN_B2)){                       // If button RB2 is pressed //Si se presiona el botón RB2
      parameter++;
      if(((i == 0) || (i == 5)) && parameter > 23)    // If hours > 23 ==> hours = 0 //Si horas> 23 ==> horas = 0
        parameter = 0;
      if(((i == 1) || (i == 6)) && parameter > 59)    // If minutes > 59 ==> minutes = 0 //Si minutos> 59 ==> minutos = 0
        parameter = 0;
      if(i == 2 && parameter > 31)               // If date > 31 ==> date = 1 // Si fecha> 31 ==> fecha = 1
        parameter = 1;
      if(i == 3 && parameter > 12)               // If month > 12 ==> month = 1 //Si mes> 12 ==> mes = 1
        parameter = 1;
      if(i == 4 && parameter > 99)               // If year > 99 ==> year = 0 // Si año> 99 ==> año = 0
        parameter = 0;
      if(i == 7 && parameter > 1)                // For alarms ON or OFF (1: alarm ON, 0: alarm OFF) // Para alarmas ON o OFF (1: alarma ON, 0: alarma OFF)
        parameter = 0;
      lcd_gotoxy(x, y);
      if(i == 7){                                // For alarms ON & OFF // Para alarmas ON y OFF
        if(parameter == 1)  lcd_putc("ON ");
        else                lcd_putc("OFF");
      }
      else
        printf(lcd_putc,"%02u", parameter);      // Display parameter //Se visualiza los parametros
      if(i >= 5){
        DS3231_read();                           // Read data from DS3231 //Leer datos de DS3231
        DS3231_display();                        // Display DS3231 time and calendar //Muestra la hora y el calendario de DS3231
      }
      delay_ms(200);                             // Wait 200ms //espera 200ms
    }
    lcd_gotoxy(x, y);                            // Go to LCD x column and y row //ir a la columna x y la fila y de la pantalla LCD
    lcd_putc("  ");                              // Print two spaces //imprime dos espacios
    if(i == 7) lcd_putc(" ");                    // Print space (for alarms ON & OFF) //Espacio de impresión (para activar y desactivar alarmas)
    blink();                                     // Call blink function //llamar a la funcion blink
    lcd_gotoxy(x, y);                            // Go to LCD x column and y row //Vaya a la columna x de la pantalla LCD y a la fila y
    if(i == 7){                                  // For alarms ON & OFF // Para alarmas ON y OFF
      if(parameter == 1)  lcd_putc("ON ");
      else                lcd_putc("OFF");
    }
    else
      printf(lcd_putc,"%02u", parameter);        // Display parameter //mostrar los parametros
    blink();
    if(i >= 5){
      DS3231_read();
      DS3231_display();}
    if((!input(PIN_B1) && i < 5) || (!input(PIN_B3) && i >= 5)){
      i++;                                       // Increment 'i' for the next parameter //Incrementar 'i' para el siguiente parámetro
      return parameter;                          // Return parameter value and exit //Devuelve el valor del parámetro y sale
    }
  }
}
void main(){
  output_b(0);
  set_tris_b(0x0F);                              // Configure RB0 ~ 3 as input pins //Configurar RB0 ~ 3 como pines de entrada
  set_tris_d(0);                                 // Configure all PORTD pins as outputs //Configurar todos los pines de PORTS como salidas
  port_b_pullups(TRUE);                          // Enable PORTB internal pull-ups //Habilitar pull-ups internos PORT
  enable_interrupts(GLOBAL);                     // Enable global interrupts //Habilitar interrupciones globales
  enable_interrupts(INT_EXT_H2L);                // Enable external interrupt with edge from high to low // Habilita la interrupción externa con borde de mayor a menor
  lcd_init();                                    // Initialize LCD module //Inicia el modulo LCD
  lcd_putc('\f');                                // LCD clear //Limpio el LCD
  reproduccion_pista(13, 1);
  while(TRUE){
    if(!input(PIN_B1)){                          // If RB1 button is pressed //Si se presiona el botón RB1
      i = 0;
      hour   = edit(hour, 1, 1);
      minute = edit(minute, 4, 1);
      while(!input(PIN_B1));                     // Wait until button RB0 released //Espere hasta que se suelte el botón RB0
      while(TRUE){
        while(!input(PIN_B1)){                   // If button RB2 button is pressed // Si se presiona el botón RB2
          day++;                                 // Increment day // Día de incremento
          if(day > 7) day = 1;
          calendar_display();                    // Call display calendar //Llama a mostraar el calendario
          lcd_gotoxy(1, 2);                      // Go to column 1 row 2  // Ir a la columna 1 fila 2
          printf(lcd_putc, calendar);            // Display calendar // Mostrar calendario
          delay_ms(200);
        }
        lcd_gotoxy(1, 2);                        // Go to column 1 row 2 // Ir a la columna 1 fila 2
        lcd_putc("   ");                         // Print 3 spaces // Imprime 3 espacios
        blink();
        lcd_gotoxy(1, 2);                        // Go to column 1 row 2 // Ir a la columna 1 fila 2
        printf(lcd_putc, calendar);              // Print calendar // Imprime calendario
        blink();                                 // Call blink function //Llama a la funcion blink         
          break;
      }
      date = edit(date, 5, 2);                   // Edit date //editar fecha
      month = edit(month, 8, 2);                 // Edit month //editar mes
      year = edit(year, 13, 2);                  // Edit year //editar año
      // Convert decimal to BCD //convierte de BCD a decimal
      minute = ((minute / 10) << 4) + (minute % 10);
      hour = ((hour / 10) << 4) + (hour % 10);
      date = ((date / 10) << 4) + (date % 10);
      month = ((month / 10) << 4) + (month % 10);
      year = ((year / 10) << 4) + (year % 10);
      // End conversion //finaliza la conversion
      // Write time & calendar data to DS3231 RTC  //Escribir datos de tiempo y calendario en DS3231 RTC
      i2c_start();                               // Start I2C protocol //Inicia el i2c
      i2c_write(0xD0);                           // DS3231 address //direccion del DS3231
      i2c_write(0);                              // Send register address (seconds address) //Envia dirección de registro (dirección de segundos)
      i2c_write(0);                              // Reset seconds and start oscillator // Restablecer los segundos e iniciar el oscilador
      i2c_write(minute);                         // Write minute value to DS3231 // Escribe el valor de los minutos en DS3231
      i2c_write(hour);                           // Write hour value to DS3231 // Escribe el valor de la hora en DS3231
      i2c_write(day);                            // Write day value // Escribe el valor del día
      i2c_write(date);                           // Write date value to DS3231 // Escribe el valor de la fecha en DS3231
      i2c_write(month);                          // Write month value to DS3231 // Escribe el valor del mes en DS3231
      i2c_write(year);                           // Write year value to DS3231 // Escribe el valor del año en DS3231
      i2c_stop();                                // Stop I2C // Detener I2C
      delay_ms(200);                             // Wait 200ms //Esperar 200ms
    }
    if(!input(PIN_B3)){                          // If RB3 button is pressed // Si se presiona el botón RB3
      while(!input(PIN_B3));                     // Wait until button RB3 released // Espere hasta que se suelte el botón RB3
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
      // Write alarms data to DS3231
      i2c_start();                               // Start I2C // Start I2C
      i2c_write(0xD0);                           // DS3231 address // Dirección DS3231
      i2c_write(7);                              // Send register address (alarm1 seconds) // Enviar dirección de registro (alarma1 segundos)
      i2c_write(0);                              // Write 0 to alarm1 seconds // Escribe 0 en alarm1 segundos
      i2c_write(alarm1_minute);                  // Write alarm1 minutes value to DS3231 // Escribe el valor de los minutos de alarma1 en DS3231
      i2c_write(alarm1_hour);                    // Write alarm1 hours value to DS3231 // Escribe el valor de las horas de alarma1 en DS3231
      i2c_write(0x80);                           // Alarm1 when hours, minutes, and seconds match // Alarma1 cuando coinciden las horas, los minutos y los segundos
      i2c_write(alarm2_minute);                  // Write alarm2 minutes value to DS3231 // Escribe el valor de la minutos de alarma2 en DS3231
      i2c_write(alarm2_hour);                    // Write alarm2 hours value to DS3231 //Escribe el valor dde hora de alarma 2 en DS3231
      i2c_write(0x80);                           // Alarm2 when hours and minutes match // Alarma2 cuando coinciden las horas y los minutos
      i2c_write(4 | alarm1_status | (alarm2_status << 1));      // Write data to DS3231 control register (enable interrupt when alarm) // Escribe datos en el registro de control DS3231 (habilita la interrupción en caso de alarma)
      i2c_write(0);                              // Clear alarm flag bits // Borrar los bits de la bandera de alarma
      i2c_stop();                                // Stop I2C // Detener I2C
      delay_ms(200);// Wait 200ms //esperar 200ms
      blink (); //(Si o si agregas esto)
    }
    if(!input(PIN_B2) && input(PIN_B4)){         // When button B2 pressed with alarm (Reset and turn OFF the alarm) // Cuando se presiona el botón B2 con alarma (Restablecer y apagar la alarma)
      output_low(PIN_B4);                        // Turn OFF the alarm indicator // Apaga el indicador de alarma
      i2c_start();                               // Start I2C // Iniciar I2C
      i2c_write(0xD0);                           // DS3231 address // Dirección DS3231
      i2c_write(0x0E);                           // Send register address (control register) // Enviar dirección de registro (registro de control)
      // Write data to control register (Turn OFF the occurred alarm and keep the other as it is)
      i2c_write(4 | (!bit_test(status_reg, 0) & alarm1_status) | ((!bit_test(status_reg, 1) & alarm2_status) << 1));
      i2c_write(0);                              // Clear alarm flag bits // Borrar los bits de la bandera de alarma
      i2c_stop();                                // Stop I2C // Detener I2C
    }
    DS3231_read();                               // Read time and calendar parameters from DS3231 RTC // Leer los parámetros de tiempo y calendario de DS3231 RTC
    alarms_read_display();                       // Read and display alarms parameters // Leer y mostrar los parámetros de las alarmas
    DS3231_display();                            // Display time & calendar // Mostrar hora y calendario
    delay_ms(50);                                // Wait 50ms // Espera 50ms
  }
}
