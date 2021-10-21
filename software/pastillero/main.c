#include <16F887.h>                      //incluimos el pic que vamos a usar
#fuses HS,NOWDT,NOPROTECT,NOLVP,PUT                       
#use delay(clock = 8MHz)
#use fast_io(B)                           //Establezco que voy a usar el puerto B (es la directiva)
#use fast_io(D)                           //Establezco que voy a usar el puerto C (es la directiva)
#use rs232(uart1, baud = 9600)
#include <lcd.c>                          //incluyo el .c del lcd
#use I2C(master, I2C1, FAST = 100000)
//#use rtos(timer = 0, minor_cycle = 20ms)
#include "modulo_de_voz.c"                //incluyo el .c del modulo de voz
#define TRIGGER_DERECHA PIN_B6
#define TRIGGER_IZQUIERDA PIN_A5
#define ECHO_DERECHA PIN_B5
#define ECHO_IZQUIERDA PIN_A6
#define CANTIDAD_MAX_DE_REPETICIONES 10
#define LED_ALARMA PIN_A3
#define BUZZER_ALARMA PIN_A2
#define DETECTOR_DE_PROXIMIDAD PIN_A4
#define CONFIG_HORA PIN_B1
#define CONFIG_ALARMAS PIN_B3
#define INCREMENTO PIN_B2
#define LED_ALARMA_2 PIN_A0
void envio_datos_alarma(int hora, int minutos, int status);
int duracion_alarma =0;                                  //declaracion de una variable
int cambiar_hora = 0;
int cambiar_alarma = 0;
int gabinete_actual =0;
char time[]     = "  :  :  ",             // declaracion de alarma tipo char (char representa caracteres individuales)
     calendar[] = "      /  /20  ",       // declaracion de alarma tipo char (char representa caracteres individuales) 
     alarm1[]   = "G1:   :  :00", alarm2[]   = "G2:   :  :00", // declaracion de alarma tipo char (char representa caracteres individuales)
     temperature[] = "T:   .   C"; // declaracion de alarma tipo char (char representa caracteres individuales)
int8  i, second, minute, hour, day, date, month, year, // declaracion de variables de 8 byts
      status_reg;                                      // declaracion de variables de 8 byts
typedef struct {
   unsigned int hora;
   unsigned int minutos;
   int status;
} DatosGabinetes;

DatosGabinetes gabinetes [3];

void reproducir_led (int led) {
   output_high(led);
   output_high(BUZZER_ALARMA);
   delay_ms(150);
   output_low(BUZZER_ALARMA);
   output_low(led);
   delay_ms(150);
}
void funcion_alarma(){
int duracion_alarma =0; 
  reproduccion_pista(gabinetes[gabinete_actual].hora, 1);
  for (duracion_alarma=1; duracion_alarma<=CANTIDAD_MAX_DE_REPETICIONES; duracion_alarma++)
   {
switch (gabinete_actual){
   case 0: 
   reproducir_led (LED_ALARMA);
   break;
   case 1:
   reproducir_led (LED_ALARMA_2);
   break;
}
   }
  clear_interrupt(INT_EXT);
}
#INT_EXT                                         // External interrupt routine // interrupcion interna routine
void ext_isr(void){
funcion_alarma();
envio_datos_alarma(gabinetes[2].hora, gabinetes[2].minutos, 1);
}
/*FunciÃ³n de lectura de datos de tiempo y calendario*/
void DS3231_read(){                              //LECTURA DE DATOS DEL DS3231
  i2c_start();                                   // empieza el i2c
  i2c_write(0xD0);                               // Direccion del DS3231
  i2c_write(0);                                  //se e  nvia la direccion de registro (registro de segundos)
  i2c_start();                                   //inicia i2c
  i2c_write(0xD1);                               // inicia la lectura de datos
  second = i2c_read(1);                          // Lee los segundos del registro 0
  minute = i2c_read(1);                          // Lee los minutos del registro 1
  hour   = i2c_read(1);                          // Lee la hora del registro 2
  day    = i2c_read(1);                          // Lee el dia del registro 3
  date   = i2c_read(1);                          // Lee la fecha del registro 4
  month  = i2c_read(1);                          // Lee el mes del registro 5
  year   = i2c_read(0);                          //Lee el aÃ±o del registro 6
  i2c_stop();                                    //Detiene el i2c 
}
/*Llama a la funcion lectura de alarmas*/
void alarms_read_display(){                      //LEER Y MOSTRAR LA FUNCION DE DATOS DE ALARMA 1 Y 2. 
  int8 control_reg, temperature_lsb;             // no se usa
  signed int8 temperature_msb;                   // no se usa
  i2c_start();                                   //inicia el protocolo I2C
  i2c_write(0xD0);                               // direccion del DS3231
  i2c_write(0x08);                               // se envia la direccion de registro (registro de los minutos alarm1)
  i2c_start();                                   //inicia i2c
  i2c_write(0xD1);                               // inicia la lectura de datos
  gabinetes[1].minutos = i2c_read(1);                   //lectura de los minutos de alarma1
  gabinetes[1].hora   = i2c_read(1);                   //lectura de las horas de alarma1
  i2c_read(1);                                   //salta el dia de alarm1
  gabinetes[2].minutos = i2c_read(1);                   //lectura de los minutos de alarma2
  gabinetes[2].hora  = i2c_read(1);                   //lectura de las horas de alarma1
  i2c_read(1);                                   //salta el dia de alarm2
  control_reg = i2c_read(1);                     //lee el control de registro de DS3231
  status_reg  = i2c_read(1);                     //Lee las estaditicas del registro DS3231
  i2c_read(1);// Lee la temperatura MSB (no se usa)
  temperature_msb = i2c_read(1);                 // Read temperature MSB
  temperature_lsb = i2c_read(0);                 // Read temperature LSB
  i2c_read(0);                 //Lee la temperatura LSB (no se usa)
  i2c_stop();                  //Detencion del i2c
  //Convierte de BCD a Decimal
  gabinetes[1].minutos = (gabinetes[1].minutos >> 4) * 10 + (gabinetes[1].minutos & 0x0F);
  gabinetes[1].hora   = (gabinetes[1].hora   >> 4) * 10 + (gabinetes[1].hora & 0x0F);
  gabinetes[2].minutos = (gabinetes[2].minutos >> 4) * 10 + (gabinetes[2].minutos & 0x0F);
  gabinetes[2].hora   = (gabinetes[2].hora   >> 4) * 10 + (gabinetes[2].hora & 0x0F);
  //Finaliza la conversion
  alarm1[8]     = gabinetes[1].minutos % 10  + 48;  
  alarm1[7]     = gabinetes[1].minutos / 10  + 48;
  alarm1[5]     = gabinetes[1].hora   % 10  + 48;
  alarm1[4]     = gabinetes[1].hora   / 10  + 48;
  alarm2[8]     = gabinetes[2].minutos % 10  + 48;
  alarm2[7]     = gabinetes[2].minutos / 10  + 48;
  alarm2[5]     = gabinetes[2].hora   % 10  + 48;
  alarm2[4]     = gabinetes[2].hora   / 10  + 48;
  gabinetes[1].status  = bit_test(control_reg, 0);      //Leer el bit de habilitaciÃ³n de interrupciÃ³n de alarma1 (A1IE) del registro de control DS3231
  gabinetes[2].status  = bit_test(control_reg, 1);      //Leer el bit de habilitaciÃ³n de interrupciÃ³n de alarma2 (A2IE) del registro de control DS3231
  if(temperature_msb < 0){
    temperature_msb = abs(temperature_msb);
    temperature[2] = '-';
  }
  else
    temperature[2] = ' ';
  temperature_lsb >>= 6;
  temperature[4] = temperature_msb % 10  + 48;
  temperature[3] = temperature_msb / 10  + 48;
  if(temperature_lsb == 0 || temperature_lsb == 2){
    temperature[7] = '0';
    if(temperature_lsb == 0) temperature[6] = '0';
    else                     temperature[6] = '5';
  }
  if(temperature_lsb == 1 || temperature_lsb == 3){
    temperature[7] = '5';
    if(temperature_lsb == 1) temperature[6] = '2';
    else                     temperature[6] = '7';
  }
  temperature[8]  = 223;                         // Degree symbol
  lcd_gotoxy(11, 1);                             // Go to column 10 row 1
  printf(lcd_putc, temperature);                 // Display temperature
  lcd_gotoxy(21, 1);                             //Ir a la columna 1 fila 3
  printf(lcd_putc, alarm1);                      //Mostrar alarma 1
  lcd_gotoxy(38, 1);                             //Va a la columna 18, fila 3
  if(gabinetes[1].status)  lcd_putc("ON ");            //Si A1IE es igual a uno muestra "on"
  else               lcd_putc("OFF");            //Si A1IE es igual a cero muestra "on"
  lcd_gotoxy(21, 2);                             //Va a la fila 4 de la columna 1
  printf(lcd_putc, alarm2);                      // Mostrar alarma2
  lcd_gotoxy(38, 2);                             // Va a la columna 18, fila 4
  if(gabinetes[2].status)  lcd_putc("ON ");            //Si A2IE es igual a uno muestra "on"
  else               lcd_putc("OFF");            //Si A2IE es igual a cero muestra "on"
  
}
void calendar_display(){                        //FUNCION QUE EDITA EL CALENDARIO Y LO MUESTRA
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
void DS3231_display(){                           //SE MUESTRA LOS DATOS DEL DS3231
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
  calendar_display();                            // Llamar a la funciÃ³n de visualizaciÃ³n del calendario
  lcd_gotoxy(1, 1);                              // Ir a la columna 1 fila 1
  printf(lcd_putc, time);                        //Mostrar la variable time
}
void blink(){
  int8 j = 0;
  while(j < 10 && (input(CONFIG_ALARMAS) || i >= 5) && input(INCREMENTO) && (input(CONFIG_HORA) || i < 5)){
    j++;
    delay_ms(25);
  }
}
int8 edit(parameter, x, y){                      //No entiendo que hace aca
  while(!input(CONFIG_ALARMAS) || !input(CONFIG_HORA));       //Espera hasta que se suelte el botÃ³n RB0
  while(TRUE){
    while(!input(INCREMENTO)){                       //Si se presiona el botÃ³n RB2
      parameter++;
      if(((i == 0) || (i == 5)) && parameter > 23)    //Si horas> 23 ==> horas = 0
        parameter = 0;
      if(((i == 1) || (i == 6)) && parameter > 59)    //Si minutos> 59 ==> minutos = 0
        parameter = 0;
      if(i == 2 && parameter > 31)               // Si fecha> 31 ==> fecha = 1
        parameter = 1;
      if(i == 3 && parameter > 12)               //Si mes> 12 ==> mes = 1
        parameter = 1;
      if(i == 4 && parameter > 99)               // Si aÃ±o> 99 ==> aÃ±o = 0
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
    if(i == 7) lcd_putc(" ");                    //Espacio de impresiÃ³n (para activar y desactivar alarmas)
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
    if((!input(CONFIG_HORA) && i < 5) || (!input(CONFIG_ALARMAS) && i >= 5)){
      i++;                                       //Incrementar 'i' para el siguiente parÃ¡metro
      return parameter;                          //Devuelve el valor del parÃ¡metro y sale
    }
  }
}
void envio_datos(){
      i2c_start();                               //Inicia el i2c
      i2c_write(0xD0);                           //direccion del DS3231
      i2c_write(0);                              //Envia direcciÃ³n de registro (direcciÃ³n de segundos)
      i2c_write(0);                              // Restablecer los segundos e iniciar el oscilador
      i2c_write(minute);                         // Escribe el valor de los minutos en DS3231
      i2c_write(hour);                           // Escribe el valor de la hora en DS3231
      i2c_write(day);                            // Escribe el valor del dÃ­a
      i2c_write(date);                           // Escribe el valor de la fecha en DS3231
      i2c_write(month);                          // Escribe el valor del mes en DS3231
      i2c_write(year);                           /// Escribe el valor del aÃ±o en DS3231
      i2c_stop();                                // Detener I2C
} 
void envio_datos_alarma(int hora, int minutos, int status){
      i2c_start();                               //Empieza el i2c
      i2c_write(0xD0);                           // DirecciÃ³n DS3231
      i2c_write(7);                              // Enviar direcciÃ³n de registro (alarma1 segundos)
      i2c_write(0);                              // Escribe 0 en alarm1 segundos
      i2c_write(minutos);                  // Escribe el valor de los minutos de alarma1 en DS3231
      i2c_write(hora);                    // Escribe el valor de las horas de alarma1 en DS3231
      i2c_write(0x80);                           // Alarma1 cuando coinciden las horas, los minutos y los segundos
      i2c_write(gabinetes[2].minutos);                  // Escribe el valor de la minutos de alarma2 en DS3231
      i2c_write(gabinetes[2].hora);                    //Escribe el valor dde hora de alarma 2 en DS3231
      i2c_write(0x80);                           // Alarma2 cuando coinciden las horas y los minutos
      i2c_write(4 | status);//  | (gabinetes[2].status  << 1)); // Escribe datos en el registro de control DS3231 (habilita la interrupciÃ³n en caso de alarma)
      i2c_write(0);                              // Borrar los bits de la bandera de alarma
      i2c_stop();                                // Detener I2C
      delay_ms(200);//esperar 200ms
      blink (); //(Si o si agregas esto)
}
void control_de_alarmas_activas(){
output_low(PIN_B4);                        // Apaga el indicador de alarma
      i2c_start();                               // Iniciar I2C
      i2c_write(0xD0);                           // DirecciÃ³n DS3231
      i2c_write(0x0E);                           // Enviar direcciÃ³n de registro (registro de control)
      // Escribir datos en el registro de control (apagar la alarma ocurrida y mantener la otra como estÃ¡)
      i2c_write(4 | (!bit_test(status_reg, 0) & gabinetes[1].status ) | ((!bit_test(status_reg, 0) & gabinetes[2].status) << 1));
      i2c_write(0);                              // Borrar los bits de la bandera de alarma
      i2c_stop();                                // Detener I2C
}
int medir_distancia(int pin_tr, int pin_echo, int renglon);
int medir_distancia(int pin_tr, int pin_echo, int renglon) {
    int check = 0;
    unsigned int16 contador = 0;
    unsigned int16 distance;
    output_high(pin_tr);
    delay_us(10);
    output_low(pin_tr);
    set_timer1(0);                                 // Reset Timer1
    while(!input(pin_echo) && (get_timer1() < 1000));
    if(get_timer1() > 990)
      check = 1;                                   // Timeout error
    set_timer1(0);                                 // Reset Timer1
    while(input(pin_echo) && (contador < 25000))
      contador = get_timer1();                            // Store Timer1 value in i 
    if(contador > 24990)                                  // Out of range error
      check = 2;
    if(check == 1){
      lcd_gotoxy(3, renglon);                            // Go to column 3 row 2
      lcd_putc("  Time Out  ");
    }
    if(check == 2){
      lcd_gotoxy(3, renglon);                     // Go to column 3 row 2
      lcd_putc(" Out Of Range");
    }
    else{
      distance = contador/58;                             // Calculate the distance
      //lcd_gotoxy(3, renglon);                     // Go to column 3 row 2
      //lcd_putc("       cm   ");
      //lcd_gotoxy(6, renglon);                            // Go to column 6 row 2
      //printf(lcd_putc,"%3Lu",distance);                               //Agregue yo (Falta agregar tris)
    }
    return distance;
}
unsigned int16 distance_1, distance_2;
void main(){
//Configuracion de puertos 
  output_low(Pin_a3);
  output_low(Pin_a2);
  output_low(pin_a4);
  output_b(0);
  set_tris_b(0b00101111);
  set_tris_d(0);                                 //Configurar todos los pines de PORTS como salidas
  set_tris_a(0b00000011);   
  port_b_pullups(TRUE);                          //Habilitar pull-ups internos PORT 
  setup_timer_1 (T1_INTERNAL | T1_DIV_BY_2);       // Configure Timer1 module
  set_timer1(0);                   // Reset Timer1

//Habilitar las interrupciones
  ext_int_edge(H_TO_L);                        
  enable_interrupts(GLOBAL);                     //Habilitar interrupciones globales
  enable_interrupts(INT_EXT);                    // Habilita la interrupciÃ³n externa con borde de mayor a menor

//Habilito el LCD
  lcd_init();                                    //Inicia el modulo LCD
   lcd_gotoxy(5,1);                               //(Cambio esta linea por el pic porque me aparece mas a la izquierda)
   printf(lcd_putc, "Bienvenido a");
   lcd_gotoxy(5,2);
   printf(lcd_putc, "*Pallm Pills*");
   delay_ms(1250);
   lcd_putc('\f');
   lcd_gotoxy(1,1);
   printf(lcd_putc, "Gracias por utilizar");
   lcd_gotoxy(3,2);
   printf(lcd_putc, "Nuestro Producto");
   delay_ms(1500);
  lcd_putc('\f'); //Limpio el LCD
  //rtos_run();                                    // Start all the RTOS tasks
  while(TRUE){
    cambiar_hora= !input(CONFIG_HORA);
    cambiar_alarma = !input(CONFIG_ALARMAS);
    if(cambiar_hora){                          //Si se presiona el botÃ³n RB1
      i = 0;
      hour   = edit(hour, 1, 1);
      minute = edit(minute, 4, 1);
      while(!input(CONFIG_HORA));                     //Espere hasta que se suelte el botÃ³n RB0
      while(TRUE){
        while(!input(CONFIG_HORA)){                   // Si se presiona el botÃ³n RB2
          day++;                                 // DÃ­a de incremento
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
      year = edit(year, 13, 2);                  //editar aÃ±o
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
    if(cambiar_alarma){                          // Si se presiona el botÃ³n RB3
      while(!input(CONFIG_ALARMAS));                     // Espere hasta que se suelte el botÃ³n RB3
      i = 5;
      gabinetes[1].hora   = edit(gabinetes[1].hora, 25, 1);
      gabinetes[1].minutos = edit(gabinetes[1].minutos, 28, 1);
      gabinetes[1].status  = edit(gabinetes[1].status , 38, 1);
      i = 5;
      gabinetes[2].hora   = edit(gabinetes[2].hora, 25, 2);
      gabinetes[2].minutos = edit(gabinetes[2].minutos, 28, 2);
      gabinetes[2].status  = edit(gabinetes[2].status , 38, 2);
      gabinetes[1].minutos = ((gabinetes[1].minutos / 10) << 4) + (gabinetes[1].minutos % 10);
      gabinetes[1].hora   = ((gabinetes[1].hora / 10) << 4) + (gabinetes[1].hora % 10);
      gabinetes[2].minutos = ((gabinetes[2].minutos / 10) << 4) + (gabinetes[2].minutos % 10);
      gabinetes[2].hora   = ((gabinetes[2].hora / 10) << 4) + (gabinetes[2].hora % 10);
      // Write alarms data to DS3231 //Escribir datos de alarmas en DS3231
    envio_datos_alarma(gabinetes[1].hora, gabinetes[1].minutos, gabinetes[1].status);
    lcd_gotoxy(10,1);
    printf(lcd_putc, "ALARMA1_h:%d", gabinetes[1].hora);
    delay_ms(1000);
    lcd_gotoxy(10,1);
    printf(lcd_putc, "ALARMA1_m:%d", gabinetes[1].minutos);
    delay_ms(1000);
    lcd_gotoxy(10,1);
    printf(lcd_putc, "ALARMA2_h:%d", gabinetes[2].hora);
    delay_ms(1000);
    lcd_gotoxy(10,1);
    printf(lcd_putc, "ALARMA2_m:%d", gabinetes[2].minutos);
    delay_ms(1000);
    lcd_putc('\f');
    }
    if(!input(INCREMENTO) && input(PIN_B4)){         // Cuando se presiona el botÃ³n B2 con alarma (Restablecer y apagar la alarma)
    control_de_alarmas_activas();
    }
    DS3231_read();                               // Leer los parÃ¡metros de tiempo y calendario de DS3231 RTC //tarea
    alarms_read_display();                       // Leer y mostrar los parÃ¡metros de las alarmas //tarea
    DS3231_display();
    /*Apartir de aca es lo del gabinete, que lo muestra en el display y podes subir y bajar los gabinetes*/
    distance_1 = medir_distancia(TRIGGER_DERECHA,ECHO_DERECHA, 1);
    distance_2 = medir_distancia(TRIGGER_IZQUIERDA, ECHO_IZQUIERDA, 2);
    if (distance_1 <= 5 || distance_2 <=5) {
      output_high(DETECTOR_DE_PROXIMIDAD);                         // Agregue yo
      delay_ms(500);                               //Agregue yo
      output_low(DETECTOR_DE_PROXIMIDAD);                          //Agregue yo
      delay_ms(500);
    }
    delay_ms(50);                                // Wait 50ms // Espera 50ms
  }
}