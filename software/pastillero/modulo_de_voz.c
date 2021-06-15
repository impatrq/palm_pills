#include <16f877a.h>
#fuses NOWDT
#fuses PUT
#fuses NOLVP
#fuses NOPROTECT
#use delay(clock = 4000000)
#use rs232(uart1, baud = 9600)

unsigned char comandoDFplayer[10] = {0x7E, 0xFF, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xEF};

void write_Comand(unsigned char *);
void dfplayer_pista(unsigned char);
void dfplayer_Pause(void);
void dfplayer_Play(void);
void dfplayer_Next(void);
void dfplayer_Prev(void);
void dfplayer_Volume(unsigned char);
void dfplayer_Volume_up(void);
void dfplayer_Volume_down(void);

void dfplayer_pista(int pist)
{
   comandoDFplayer[3] = (unsigned char)0x03; // {0x7E, 0xFF, 0x06, 0x03, 0x00, 0x00, 0x01, 0xFE, 0x79, 0xEF}; (si seria la pista N°1 sino cambio el bit 6 para otra pista)
   comandoDFplayer[5] = (unsigned char)0x00;
   comandoDFplayer[6] = (unsigned char)pist;
   write_Comand(comandoDFplayer); // llama la funcion comand write para calcular el checksum, entonces sumo del 0x7E al bit 6, para luego restarselo a 0xFFFF que el resultado se guarda en el Bit 7 y 8
                                  // 187 da la suma. Y la resta da FE78. Y luego se le suma 1 =FE79.
}

void dfplayer_Pause(void)
{
   comandoDFplayer[3] = (unsigned char)0x0E; // {0x0E, 0xFF, 0x06, 0x00, 0x00, 0x00, 0x00, 0xFE, 0xED, 0xEF}
   comandoDFplayer[5] = (unsigned char)0x00;
   comandoDFplayer[6] = (unsigned char)0x00;
   write_Comand(comandoDFplayer); // llama la funcion comand write para calcular el checksum, entonces sumo del 0x7E al bit 6, para luego restarselo a 0xFFFF que el resultado se guarda en el Bit 7 y 8
                                  //La suma da 113. Y la resta FEEC. y le sumo 1 =FEED
}

void dfplayer_Play(void)
{
   comandoDFplayer[3] = (unsigned char)0x0D; // {0x7E, 0xFF, 0x06, 0x00, 0x00, 0x00, 0x00, 0xFE, 0x7D, 0xEF};
   comandoDFplayer[5] = (unsigned char)0x00;
   comandoDFplayer[6] = (unsigned char)0x00;
   write_Comand(comandoDFplayer); // llama la funcion comand write para calcular el checksum, entonces sumo del 0x7E al bit 6, para luego restarselo a 0xFFFF que el resultado se guarda en el Bit 7 y 8
                                  // La suma da 183. Y la resta FE7C. Y le sumo 1 =FE7D
}

void dfplayer_Next(void)
{
   comandoDFplayer[3] = (unsigned char)0x01; // {0x7E, 0xFF, 0x06, 0x01, 0x00, 0x00, 0x00, 0xFE, 0x7C, 0xEF};
   comandoDFplayer[5] = (unsigned char)0x00;
   comandoDFplayer[6] = (unsigned char)0x00; // La suma 184. Y la resta da FE7B. Y le sumo 1 =FE7C
   write_Comand(comandoDFplayer);
}

void dfplayer_Prev(void)
{
   comandoDFplayer[3] = (unsigned char)0x02; // {0x7E, 0xFF, 0x06, 0x02, 0x00, 0x00, 0x00, 0xFE, 0x7B, 0xEF};
   comandoDFplayer[5] = (unsigned char)0x00;
   comandoDFplayer[6] = (unsigned char)0x00; // La suma es 185. Y la resta da FE7A. Y le sumo 1 =FE7B
   write_Comand(comandoDFplayer);
}

void dfplayer_Volume(unsigned char volumen)
{
   comandoDFplayer[3] = (unsigned char)0x06; // {0x7E, 0xFF, 0x06, 0x06, 0x00, 0x00, 0x09, 0xFE, 0x6E, 0xEF}; (Subo el volumen a 9, por eso cambiaa en el byte 6 a 9)
   comandoDFplayer[5] = (unsigned char)0x00;
   comandoDFplayer[6] = (char)volumen; // la suma es 192. Y la resta es FE6D. Y le sumo 1 = FE6E
   write_Comand(comandoDFplayer);
}

void dfplayer_Volume_up(void)
{
   comandoDFplayer[3] = (unsigned char)0x04; // {0x7E, 0xFF, 0x06, 0x04, 0x00, 0x00, 0x00, 0xFE, 0x89, 0xEF};
   comandoDFplayer[5] = (unsigned char)0x00;
   comandoDFplayer[6] = (unsigned char)0x00; // La suma es 177. Y la resta es FE88  Y le sumo 1 = FE89
   write_Comand(comandoDFplayer);
}

void dfplayer_Volume_down(void)
{
   comandoDFplayer[3] = (unsigned char)0x05; // {0x7E, 0xFF, 0x06, 0x05, 0x00, 0x00, 0x00, 0xFE, 0x78, 0xEF};
   comandoDFplayer[5] = (unsigned char)0x00;
   comandoDFplayer[6] = (unsigned char)0x00; // La suma es 188. Y la resta es FE77. Y le sumo 1 = FE78
   write_Comand(comandoDFplayer);
}
void write_Comand(unsigned char *cmd)
{
   volatile static unsigned int16 checksum = 0;
   volatile static unsigned char i = 0;
   checksum = 0;
   for (i = 1; i < 7; i++)
   {
      checksum += cmd[i];
   }
   checksum = 0xFFFF - checksum + 1; //calcular esto
   cmd[7] = (unsigned char)(checksum >> 8);
   cmd[8] = (unsigned char)checksum;
   for (i = 0; i < 10; i++)
   {
      putchar(cmd[i]);
   }
}

int calculo_de_posicion(int hora,int gabinete) {
   
int inicio = (gabinete - 1)*24;
int posicion = inicio + hora; 
return posicion; 
}
void reproduccion_pista(int hora, int gabinete){
   int posicion = calculo_de_posicion(hora,gabinete);
   dfplayer_pista(posicion);
   dfplayer_play();
   delay_ms(5000);
   return;
}
void main()
{
   int i, j;  
   delay_ms(2000);
   dfplayer_play();
   dfplayer_pista(1);
   dfplayer_play();
   delay_ms(2000);
   dfplayer_next();
   delay_ms(2000);
   dfplayer_prev();
   delay_ms(4000);

   for (i = 0; i < 15; i++)
   {
      dfplayer_Volume_down();
      delay_ms(200);
   }

   for (j = 0; j < 15; j++)
   {
      dfplayer_Volume_up();
      delay_ms(200);
   }
}