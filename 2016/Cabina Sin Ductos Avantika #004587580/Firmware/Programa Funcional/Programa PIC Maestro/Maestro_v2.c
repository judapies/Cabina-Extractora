// Comunicación USB para control de fermentador.
// Reloj con XT de 4 MHz.
// Programación para Fermentador de 7 Litros
// Tiene Control: pH,Oxigeno Disuelto, CO2, Temperatura, Agitación y Foam.
// Tiene Control de cuatro bombas peristalticas: Acido, Base, Foam y Medio.
// Interfaz de control realizada en PC touch todo en uno Lenovo.
// Desarrollo de la interfaz en Java NetBeans.
// Ing. Juan David Piñeros Espinosa.
// Ing. Faiver Humberto Trujillo.
// JP Inglobal. 2015

#include <18F4550.h>
#device adc=10
#fuses HSPLL,NOWDT,NOPROTECT,NOLVP,NODEBUG,USBDIV,PLL1,CPUDIV1,VREGEN,NOMCLR
#use delay(clock=48000000)
#use i2c(Master,fast,sda=PIN_B0,scl=PIN_B1)
#include "HDM64GS12.c"
#include "GRAPHICS.c"
#include "imagen.h"

#define   UP               input(PIN_A0)
#define   DOWN             input(PIN_A1)
#define   RIGHT            input(PIN_A2)
#define   LEFT             input(PIN_A3)

#define   Display_on       output_bit(PIN_E0,1)
#define   Display_off      output_bit(PIN_E0,0)

const int8 Lenbuf = 32;
int8 txbuf[Lenbuf];

int16 tiempos=0;
int8 i=0;
byte SlaveA0Tx[0X10];
byte dato=0;            //Contendrá la información a enviar o recibir
byte direccion=0;       //Contendrá la dirección del esclavo en el bus I2C
byte posicion=0;        //Contendrá la posición en el buffer de memoria del esclavo donde se almacenará o de donde se recuperará el dato

short Flanco=0,Flanco1=0,Flanco2=0,Flanco3=0,Flanco4=0,Flanco5=0,Flanco6=0,imprimir=0,flag_uv=0,flag_blower=0,flag_toma=0,flag_luz=0,flag_alarma=0,w=0;
short tiempo_purga=0,tiempo_uv=0,tiempo_trabajo=0,tiempo_postpurga=0,Latencia=0,flag_latencia=0;
short guardauv=0,guardatrabajo=0,Lectura=0,n=0;
char  JP[] = "JP Inglobal", Cabina[] = "Cabina Extractora",tiempo_est[]="T est=", Dos_puntos[]=":";
char  Clase[] = "Sin Ductos", Tiempo[] = "Tiempo de Purga", UV[]="UV";
char  Baje_Vidrio[] = "!!Ubique Vidrio!!",Inflowt[] = "JPCEGH-F-A",Downflowt[] = "Inflow(m/s):", ZF[] = "Fabrica:203.0", ZA[] = "Actual:",AJ[]="Ajuste:";
char  Vida_UV[] = "UVLife:",Vida_Filtro[] = "Filter:",ZFP[] = "Fabrica:235.0", Perdida[]="!Perdida Flujo!";
int8 Menu=200, unidad=1, Opcion=1,inicie=0,negativo=10,MediaMovil=5,G_l=0,G_h=0,tiemporeset=0;
int8 q=0,r=0,l=0,h=0,Entero=0,Decimal1=0,LuzBlanca=10,LuzUV=10,Motor=10,Alarma=10,Toma=10,UV2=0;
unsigned int8 Inflow_l=0,Inflow_h=0,Downflow_h=0,Downflow_l=0,Vidrio_abajo=0,Vidrio_ok=0,Filtro_Inflow_l=0,Filtro_Inflow_h=0;
unsigned int8 Filtro_Downflow_l=0,Filtro_Downflow_h=0;

unsigned int16 Inflow_adc=0,Downflow_adc=0,Filtro_Inflow_adc=0,Filtro_Downflow_adc=0,Temporal1=0;

int16 t_latencia=0,minutos_uv=0,minutos_trabajo=0,G16=0;
signed int  Tpurga[4]={0,0,0,0};       // Tiempo de Purga transcurrido
signed int  Tpurgap[4]={9,0,0,0};      // Tiempo de Purga programado
signed int  Tppurga[4]={0,0,0,0};      // Tiempo de Post-Purga transcurrido
signed int  Tppurgap[4]={9,0,0,0};     // Tiempo de Post-Purga programado
signed int  Tuv[4]={0,0,0,0};          // Tiempo de UV transcurrido(en Horas)
signed int  Tempouv[4]={0,0,0,0};         // Tiempo de UV transcurrido
signed int  Tempouvp[4]={0,0,0,0};        // Tiempo de UV transcurrido
signed int  Ttrabajo[4]={0,0,0,0};        // Tiempo de trabajo transcurrido (en Horas)
signed int  Contrasena[4]={0,0,0,0};      // Contrasena de 4 digitos
signed int  Password[4]={0,0,0,0};        // Contrasena Almacenada de 4 digitos
float Inflow=0.0,Downflow=0.0,Filtro_Downflow=0.0,Diferencia=0.0,barra=0.0,barra_old=0.0,barra2=0.0,barra_old2=0.0,Inflowdec=0.0;
float Velocidad=0.0,x_uno=0.0,x_cuadrado=0.0,x_cubo=0.0,Ajuste1=0.0,Ajuste2=0.0;
float ADC=0,Temporal=0.0,Temporal2=0.0;
float a=0.0000001418633343546420,b=-0.00009476749001431169,c= 0.040182822903506,d= -5.354852229527197;//Sensor 2 Posible bueno
float promedio[8]={0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
float PromPresion[8]={0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
float Flujo2=0.0,zero_fabrica=203.0,zero_actual=0.0;
float V1=0.0,Presion=0.0,G=2.0,UVTime=0.0;

typedef struct{
   char Letra[20];
}MENUU;

MENUU Menus;

void glcd_imagen(int8 x)
{
   char i,j;
   signed char k; 
     
   if(x==3){
   for( i = 0 ; i < 64 ; i ++ )
   {  for( j = 0 ; j < 16 ; j ++)
      {  for(k=7;k>-1;k--)
         {      
            if( bit_test(Bombillo[i][j] ,7-k )) 
               glcd_pixel( j*8+k,i, ON );
         }  
      }
   }
   }
   
   if(x==4){
   for( i = 0 ; i < 64 ; i ++ )
   {  for( j = 0 ; j < 16 ; j ++)
      {  for(k=7;k>-1;k--)
         {      
            if( bit_test(Tomai[i][j] ,7-k )) 
               glcd_pixel( j*8+k,i, ON );
         }  
      }
   }
   }
   
   if(x==5){
   for( i = 0 ; i < 64 ; i ++ )
   {  for( j = 0 ; j < 16 ; j ++)
      {  for(k=7;k>-1;k--)
         {      
            if( bit_test(Ventilador[i][j] ,7-k )) 
               glcd_pixel( j*8+k,i, ON );
         }  
      }
   }
   }
}

void mensajes(int8 x,y){// Funcion para imprimir mensajes de Menu Principal.
  if(x==1)
   {glcd_text57(7, y, tiempo_est, 1, ON);}
}
 
void displayContrasena(int digito, int x, int y, int sombreado)
{
   char voltage[9];
   sprintf(voltage, "%i", digito);
   
   if(sombreado==1)
      {glcd_rect(x, y, x+18, y+24, YES, ON);glcd_text57(x+2, y+1, voltage, 2, OFF);}
   else   
      {glcd_rect(x, y, x+18, y+24, NO, OFF);glcd_text57(x+2, y+1, voltage, 2, ON);}

}

void displayTiempo(int digito, int x, int y, int sombreado, int tamano)
{
   char voltage[9];
   sprintf(voltage, "%i", digito);
   
   if(sombreado==1)
      {glcd_rect(x, y, x+(tamano*6), y+(tamano*8), YES, ON);glcd_text57(x+2, y+1, voltage, tamano, OFF);}
   if(sombreado==0)
      {glcd_rect(x, y, x+(tamano*6), y+(tamano*8), YES, OFF);glcd_text57(x+2, y+1, voltage, tamano, ON);}

}

void displayMenu(char palabra[100], int x, int y, int sombreado, int tamano)
{
   strcpy(Menus.Letra,palabra);
   if(sombreado==1)
      {glcd_rect(0, y, 127, y+(tamano*8), YES, ON);glcd_text57(x+1, y+1, Menus.Letra, tamano, OFF);}
   if(sombreado==0)
      {glcd_rect(0, y, 127, y+(tamano*8), YES, OFF);glcd_text57(x+1, y+1, Menus.Letra, tamano, ON);}

}

void displayfloat(float adc,int x,int y,int sombreado, int tamano) {
   char voltage[9];    
      sprintf(voltage, "%1.2f",adc); // Converts adc to text
      
   if(sombreado==1)
      {glcd_rect(x, y, x+(tamano*48), y+(tamano*8), YES, ON);glcd_text57(x+2, y+1, voltage, tamano, OFF);}
   if(sombreado==0)
      {glcd_rect(x, y, x+(tamano*48), y+(tamano*8), YES, OFF);glcd_text57(x+2, y+1, voltage, tamano, ON);}
      
}

Float Leer_Sensor_Flujo(int media)
{
    if(negativo==10)
    {
      x_uno=Inflow_adc+Diferencia;
    }
    if(negativo==20)
    {
      x_uno=Inflow_adc-Diferencia;
    }
    x_cuadrado=x_uno*x_uno;
    x_cubo=x_uno*x_cuadrado;
    Velocidad=(x_cubo*a)+(x_cuadrado*b)+(x_uno*c)+d; 
    //Velocidad=Velocidad*correccion;//Ajuste de Temperatura
    Velocidad=Velocidad-0.08;
    if(Velocidad<0.0)
     {Velocidad=0.0;}
     
     if(l>=media)
       {l=0;}
     
     promedio[l]=Velocidad;l++;
     Flujo2=0;
        for(h=0;h<=(media-1);h++)
        {
           Flujo2+=promedio[h];
        } 
           Flujo2=(Flujo2/media)*Ajuste1; 
           return Flujo2;
}

float Leer_Sensor_Presion(int media){
   float promediopresion=0.0;
   
   V1=Filtro_Downflow_adc; 
   //V1 = (x_uno*5.0)/1023.0;   //Lectura de Divisor de Voltaje de PT100 con resistencia de 1k (+-10%)
   Presion=(V1/G)-1.0;// Presion=(Voltaje/Ganancia)-1
   
   if(Presion<0.0)
   {
      Presion=0.0;
   }

   if(r>media-1)
   {r=0;}
   PromPresion[r]=Presion*4.02;r++;
         
   for(q=0;q<=(media-1);q++)
   {
      promediopresion+=PromPresion[q];
   } 
   promediopresion=promediopresion/media;   
   
   return promediopresion;
}

#int_TIMER1
void temp1s(void){
   set_timer1(5536);
   tiempos++;
   
   if(flag_latencia==1)
   {
      t_latencia++;         
   }
   
   if(t_latencia>=3000)
   {Latencia=1;}
                
   if(tiempos>=200)
   {
      tiempos=0;imprimir=1;inicie++;Lectura=1;tiemporeset++;
   
   if(tiempo_postpurga)
   {
      Tppurga[0]++;
      if(Tppurga[0]>9)
      {
         Tppurga[0]=0;
         Tppurga[1]++;
      }
      if(Tppurga[1]>5)
      {  
         Tppurga[1]=0;
         Tppurga[2]++;
      }
      if(Tppurga[2]>9)
      {
         Tppurga[2]=0;
         Tppurga[3]++;
      }
      if(Tppurga[3]>5)
      {
         Tppurga[3]=0;
      }
   }
   
   if(flag_blower==1)
   {
      minutos_trabajo++; 
   }
  
   if(tiempo_purga==1)
   {                  
      Tpurga[0]++;
      if(Tpurga[0]>9)
      {
         Tpurga[0]=0;
         Tpurga[1]++;
      }
      if(Tpurga[1]>5)
      {
         Tpurga[1]=0;
         Tpurga[2]++;
      }
      if(Tpurga[2]>9)
      {
         Tpurga[2]=0;
         Tpurga[3]++;
      }
      if(Tpurga[3]>5)
      {
         Tpurga[3]=0;
      }
   }       
        
   }
}

void Envio_I2C(direccion, posicion, dato){

   i2c_start();            // Comienzo comunicación
   i2c_write(direccion);   // Dirección del esclavo en el bus I2C
   i2c_write(posicion);    // Posición donde se guardara el dato transmitido
   i2c_write(dato);        // Dato a transmitir
   i2c_stop();             // Fin comunicación
 }

void Lectura_I2C (byte direccion, byte posicion, byte &dato) {

   i2c_start();            // Comienzo de la comunicación
   i2c_write(direccion);   // Dirección del esclavo en el bus I2C
   i2c_write(posicion);    // Posición de donde se leerá el dato en el esclavo
   i2c_start();            // Reinicio
   i2c_write(direccion+1); // Dirección del esclavo en modo lectura
   dato=i2c_read(0);       // Lectura del dato
   i2c_stop();             // Fin comunicación
}

void Lee_Vector(void){
   Inflow_l=txbuf[0];
   Inflow_h=txbuf[1];
   Downflow_l=txbuf[2];
   Downflow_h=txbuf[3];
   Vidrio_abajo=txbuf[4];
   Vidrio_ok=txbuf[5];
   Filtro_Inflow_l=txbuf[8];
   Filtro_Inflow_h=txbuf[9];
   Filtro_Downflow_l=txbuf[6];
   Filtro_Downflow_h=txbuf[7];
   
   Inflow_adc=make16(Inflow_h,Inflow_l);
   Downflow_adc=make16(Downflow_h,Downflow_l);
   
   Filtro_Inflow_adc=make16(Filtro_Inflow_h,Filtro_Inflow_l);
   Filtro_Downflow_adc=make16(Filtro_Downflow_h,Filtro_Downflow_l);
}

void Carga_Vector(void){
   SlaveA0Tx[0]=LuzBlanca;
   SlaveA0Tx[1]=LuzUV;
   SlaveA0Tx[2]=Motor;
   SlaveA0Tx[3]=Toma;
   SlaveA0Tx[4]=Alarma;
   SlaveA0Tx[5]=(int8)Inflow;
   Inflowdec=100*(Inflow-(int8)Inflow);
   SlaveA0Tx[6]=(int8)Inflowdec;
}

void Envio_Esclavos(void){
   Carga_Vector();
   for(i=0;i<7;i++)
   {
      direccion=0xA0;
      Envio_I2C(direccion,i,SlaveA0Tx[i]);
   } 
}
void Lectura_Esclavos(void){
   // Lectura
   for(i=0;i<10;i++)
   {
      direccion=0xA0;                        //Dirección en el bus I2c
      posicion=i;                         //Posición de memoria a leer
      Lectura_I2C(direccion, posicion, dato);    //Lectura por I2C
      txbuf[i]=(int8)dato;
      //Muestra en el lcd las variables de la transmisión
      //lcd_gotoxy(1,1);
      //printf(lcd_putc, "\fI2C=%x Pos.%d =%x" direccion, posicion, dato);
      //delay_ms (1000); 
   }
   Lee_Vector();
   Inflow=Leer_Sensor_Flujo(MediaMovil);
   Filtro_Downflow=Leer_Sensor_Presion(MediaMovil);
}

void main(){

   for (posicion=0;posicion<0x10;posicion++)
   {
      SlaveA0Tx[posicion] = 0x00;
   }

   setup_timer_1(T1_INTERNAL|T1_DIV_BY_1);
   enable_interrupts(INT_TIMER1);
   enable_interrupts(global);   
   
   
   Password[0]=read_eeprom(0);Password[1]=read_eeprom(1);Password[2]=read_eeprom(2);Password[3]=read_eeprom(3);
   Tpurgap[0]=read_eeprom(4);Tpurgap[1]=read_eeprom(5);Tpurgap[2]=read_eeprom(6);Tpurgap[3]=read_eeprom(7);
   Tppurgap[0]=read_eeprom(8);Tppurgap[1]=read_eeprom(9);Tppurgap[2]=read_eeprom(10);Tppurgap[3]=read_eeprom(11);
   Tuv[0]=read_eeprom(12);Tuv[1]=read_eeprom(13);Tuv[2]=read_eeprom(14);Tuv[3]=read_eeprom(15);
   Ttrabajo[0]=read_eeprom(16);Ttrabajo[1]=read_eeprom(17);Ttrabajo[2]=read_eeprom(18);Ttrabajo[3]=read_eeprom(19);
   flag_alarma=read_eeprom(20);
   Tempouvp[0]=read_eeprom(21);Tempouvp[1]=read_eeprom(22);Tempouvp[2]=read_eeprom(23);Tempouvp[3]=read_eeprom(24);
   
   Entero=read_eeprom(25);//Decimal1=read_eeprom(26);
   Temporal=read_eeprom(26);
   Temporal=Temporal/100.0;
   Ajuste1=Entero+Temporal;
   
   
   Entero=read_eeprom(27);//Decimal1=read_eeprom(26);
   Temporal=read_eeprom(28);
   Temporal=Temporal/100.0;
   Ajuste2=Entero+Temporal;
   
   
   Entero=read_eeprom(29);//Decimal1=read_eeprom(26);
   Temporal=read_eeprom(30);
   Temporal=Temporal/100.0;
   zero_actual=Entero+Temporal;
   
   Diferencia=make16(read_eeprom(61),read_eeprom(60));
   G=make16(read_eeprom(63),read_eeprom(62));
   negativo=read_eeprom(50);

   while(true){

      Envio_Esclavos();
   
      glcd_update();
   
   if(minutos_trabajo>=3600) // Tiempo de Trabajo
   {
      minutos_trabajo=0;
      Ttrabajo[0]++;
      if(Ttrabajo[0]>9)
      {
         Ttrabajo[0]=0;
         Ttrabajo[1]++;
      }
      if(Ttrabajo[1]>9)
      {
         Ttrabajo[1]=0;
         Ttrabajo[2]++;
      }
      if(Ttrabajo[2]>9)
      {
         Ttrabajo[2]=0;
         Ttrabajo[3]++;
      }
      if(Ttrabajo[3]>9)
      {
         Ttrabajo[3]=0;
      }
   }          
   
   if(Menu!=2)
      Alarma=10;
//_--------------------------------------------------------------------------------------------------------------------------------------------------------
      if(Menu==0){ //Menu de Contraseña.
      glcd_rect(0, 0, 127, 25, YES, ON);strcpy(Menus.Letra,"Clave");glcd_text57(25, 1, Menus.Letra, 3, OFF);   
      flag_latencia=1;
      
      if(UP)//Si oprime hacia arriba
      {
         flag_latencia=0;t_latencia=0;
         if(Flanco == 0)
         {
            Flanco = 1;delay_ms(30);
            for(i=1;i<=4;i++)
            {
               if(unidad==i)
               {
                  Contrasena[i-1]++;
                  if(Contrasena[i-1]>9)
                  {
                     Contrasena[i-1]=0;
                  }
                  displayContrasena(Contrasena[i-1],i*20,30,1);
               }
               else
               {
                  displayContrasena(Contrasena[i-1],i*20,30,0);
               }
            }
         }
      }
      else
      {
         Flanco = 0;
      }
            
      if(DOWN)//Si oprime hacia abajo
      {
         flag_latencia=0;t_latencia=0;
         if(Flanco2 == 0)
         {
            Flanco2 = 1;delay_ms(30);
            for(i=1;i<=4;i++)
            {
               if(unidad==i)
               {
                  Contrasena[i-1]--;
                  if(Contrasena[i-1]<0)
                  {
                     Contrasena[i-1]=9;}
                     displayContrasena(Contrasena[i-1],i*20,30,1);
               }
               else
               {
                  displayContrasena(Contrasena[i-1],i*20,30,0);
               }
            }      
         }     
      }
      else
      {
         Flanco2 = 0;
      }
            
      if(RIGHT)//Si oprime SET
      {
         flag_latencia=0;t_latencia=0;
         if(Flanco1 == 0)
         {
            Flanco1 = 1;delay_ms(30);unidad++;
            for(i=1;i<=4;i++)
            {
               if(unidad==i)
               {
                  displayContrasena(Contrasena[i-1],i*20,30,1);
               }
               else
               {
                  displayContrasena(Contrasena[i-1],i*20,30,0);
               }
            }
         }
      }
      else
      {
         Flanco1 = 0;
      }
    
      if(unidad>4)
      {
         glcd_fillScreen(OFF);unidad=4;flag_latencia=0;t_latencia=0;
         if(Contrasena[0]==3&&Contrasena[1]==8&&Contrasena[2]==9&&Contrasena[3]==2) // Si Ingresa clave para reset general del sistema.
         {
            write_eeprom(0,0);delay_ms(20);write_eeprom(1,0);delay_ms(20);// Reestablece a contraseña de Fabrica y reinicia Programa.
            write_eeprom(2,0);delay_ms(20);write_eeprom(3,0);delay_ms(20);
            reset_cpu();
         }
      
         if((Contrasena[0]==Password[0])&&(Contrasena[1]==Password[1])&&(Contrasena[2]==Password[2])&&(Contrasena[3]==Password[3]))
         {
            glcd_fillScreen(OFF);
            strcpy(Menus.Letra,"Clave");
            displayMenu(Menus.Letra,30,0,0,2);
            strcpy(Menus.Letra,"Correcta");
            displayMenu(Menus.Letra,15,30,0,2);
            Menu=1;glcd_update();delay_ms(1000);glcd_fillScreen(OFF);
            Contrasena[0]=0;Contrasena[1]=0;Contrasena[2]=0;Contrasena[3]=0;
            displayTiempo(Tpurga[3],20,20,0,2);displayTiempo(Tpurga[2],40,20,0,2);glcd_text57(60, 22, Dos_puntos, 2, ON);
            displayTiempo(Tpurga[1],70,20,0,2);displayTiempo(Tpurga[0],90,20,0,2);
            enable_interrupts(global);tiempo_purga=1;
         }// Esta parte se puede agregar en el Menu 1 dependiendo de la ubicación del vidrio.
         else
         {
            glcd_fillScreen(OFF);
            strcpy(Menus.Letra,"Clave");
            displayMenu(Menus.Letra,30,0,0,2);
            strcpy(Menus.Letra,"Incorrecta");
            displayMenu(Menus.Letra,0,30,0,2);
            unidad=1;glcd_update();delay_ms(1000);Contrasena[0]=0;Contrasena[1]=0;Contrasena[2]=0;Contrasena[3]=0;glcd_fillScreen(OFF);
            glcd_rect(0, 0, 127, 25, YES, ON);strcpy(Menus.Letra,"Clave");glcd_text57(25, 1, Menus.Letra, 3, OFF);displayContrasena(Contrasena[0],20,30,1);
            displayContrasena(Contrasena[1],40,30,0);displayContrasena(Contrasena[2],60,30,0);displayContrasena(Contrasena[0],80,30,0);
         }
      }
      }
//_--------------------------------------------------------------------------------------------------------------------------------------------------------       

//_--------------------------------------------------------------------------------------------------------------------------------------------------------
      if(Menu==1)//Menu de Purga.
      { 
         glcd_text57(15, 1, Tiempo, 1, ON);
         Lectura_Esclavos();
         // Para que inicie el tiempo de purga se debe tener el vidrio en la altura adecuada.
         // Una vez ubicado el vidrio se debe encender la Luz Blanca y el Ventilador e iniciar el Tiempo de purga.
         //Motor(1);
         tiempo_purga=1;LuzBlanca=10;LuzUV=10;Toma=5;Alarma=10;Motor=5;
         glcd_rect(9, 54, 120, 64, YES, OFF);
         
         displayTiempo(Tpurga[3],20,20,0,2);displayTiempo(Tpurga[2],40,20,0,2);glcd_text57(60, 22, Dos_puntos, 2, ON);
         displayTiempo(Tpurga[1],70,20,0,2);displayTiempo(Tpurga[0],90,20,0,2);        
      
         if(RIGHT)
         {
            delay_ms(1000);
            if(RIGHT)
            {
               tiempo_purga=0;Menu=2;glcd_fillScreen(OFF);
            }
         }
       
         if((Tpurga[0]>=Tpurgap[0])&&(Tpurga[1]>=Tpurgap[1])&&(Tpurga[2]>=Tpurgap[2])&&(Tpurga[3]>=Tpurgap[3]))
         {
            tiempo_purga=0;Menu=2;glcd_fillScreen(OFF);flag_luz=!flag_luz;flag_blower=1;
         }      
      }
//_--------------------------------------------------------------------------------------------------------------------------------------------------------       

//_--------------------------------------------------------------------------------------------------------------------------------------------------------
      if(Menu==2) //Menu General.
      {         
         w=0;
         glcd_imagen(4);
         glcd_text57(0, 1, Inflowt, 1, ON);
         glcd_text57(0, 21, Downflowt, 1, ON);
         glcd_text57(0, 35, Vida_Filtro, 1, ON);
         //displayTiempo(Ttrabajo[3],60,28,0,1);displayTiempo(Ttrabajo[2],67,28,0,1);displayTiempo(Ttrabajo[1],74,28,0,1);displayTiempo(Ttrabajo[0],82,28,0,1);
         //glcd_text57(0, 43, Vida_UV, 1, ON);
         //displayTiempo(Tuv[3],60,42,0,1);displayTiempo(Tuv[2],67,42,0,1);displayTiempo(Tuv[1],74,42,0,1);displayTiempo(Tuv[0],82,42,0,1);       
         //UVTime=Tuv[0]+(Tuv[1]*10)+(Tuv[2]*100)+(Tuv[3]*1000);         
         
         if(imprimir==1)
         {
            Lectura_Esclavos();
            Downflow=Inflow;
            //displayfloat(Inflow,70,1,0,1);
            displayfloat(Downflow,70,21,0,1);  
            imprimir=0;
         }
         
         if(flag_blower==1)
         {
            glcd_imagen(5);tiempo_trabajo=1;Motor=5;Toma=5;
            if(guardatrabajo==1)
            {
               guardatrabajo=0;write_eeprom(30,minutos_trabajo);
               write_eeprom(16,Ttrabajo[0]);write_eeprom(17,Ttrabajo[1]);write_eeprom(18,Ttrabajo[2]);write_eeprom(19,Ttrabajo[3]);
            }
         }
         else
         {
            Filtro_Downflow=0;
            glcd_rect(102, 45, 127, 64, YES, OFF);tiempo_trabajo=0;Motor=10;Toma=10;
            if(guardatrabajo==0)
            {
               guardatrabajo=1;write_eeprom(30,minutos_trabajo);
               write_eeprom(16,Ttrabajo[0]);write_eeprom(17,Ttrabajo[1]);write_eeprom(18,Ttrabajo[2]);write_eeprom(19,Ttrabajo[3]);
            }
         }
         
// Dibujo de barra de estado de filtro                  
         barra=100.0*(Filtro_Downflow/2.0);
         if(barra>100.0)
            barra=100.0;
         barra=barra/2;
         glcd_rect(42, 34, (int8)(barra_old)+42, 41, YES, OFF);  // Clears the old bar
         glcd_rect(42, 34, (int8)(barra)+42, 41, YES, ON);             // Draws a new bar
         glcd_rect(40, 32, 94, 43, NO, ON);             // Draws a new bar
         barra_old=barra;
// Fin de Dibujo de barra de estado de Filtro
/*
// Dibujo de barra de estado de Luz UV
         barra2=100.0*(UVTime/1000.0);
         if(barra2>100.0)
            barra2=100.0;
         barra2=barra2/2;
         glcd_rect(42, 42, (int8)(barra_old2)+42, 49, YES, OFF);  // Clears the old bar
         glcd_rect(42, 42, (int8)(barra2)+42, 49, YES, ON);             // Draws a new bar
         glcd_rect(40, 40, 94, 51, NO, ON);             // Draws a new bar
         barra_old2=barra2;
// Fin de Dibujo de barra de estado de Luz UV
*/

         if(Downflow<0.50)
         {
            flag_latencia=1;
            if(Latencia==1)
            {         
               //strcpy(Menus.Letra,"!Perdida Flujo!");
               //displayMenu(Menus.Letra,0,55,0,1);
               if(Downflow<0.50)
               {
                  Alarma=5;
               }
            }
         }
         else
         {
            Latencia=0;t_latencia=0;flag_latencia=0;
            glcd_rect(0, 54, 104, 64, YES, OFF);Alarma=10;
         }
      
         if(flag_luz==1)
         {
            glcd_imagen(3);LuzBlanca=5;
         }
         else
         {
            glcd_rect(102, 14, 127, 28, YES, OFF);LuzBlanca=10;
         }
         
         
         if(Alarma==5)
         {
            glcd_rect(0, 55, 100, 63, YES, OFF);
            glcd_text57(0,55, Perdida, 1, ON);
         }
         else
         {               
            glcd_rect(0, 55, 100, 63, YES, OFF);            
         }
            
         if(RIGHT)//Si oprime boton de Blower.
         {
            if(Flanco3 == 0)
            {
               Flanco3 = 1;delay_ms(30);flag_blower=!flag_blower;
            }
         }
         else
         {
            Flanco3 = 0;
         }
               
         if(LEFT)//Si oprime boton de Luz UV.
         {
            if(Flanco4 == 0)
            {
               Flanco4 = 1;delay_ms(30);flag_uv=!flag_uv;
               if(UV2==0)
                  UV2=10;
               else
                  UV2=0;
            }
         }
         else
         {
            Flanco4 = 0;
         }
               
         if(DOWN)//Si oprime boton de Luz Blanca.
         {
            if(Flanco5 == 0)
            {
               Flanco5 = 1;delay_ms(30);flag_luz=!flag_luz;
            }
         }
         else
         {
            Flanco5 = 0;
         }
               
         if(UP)//Si oprime boton de Toma.
         {
            delay_ms(500);
            if(UP)//Si oprime boton de Toma.
            {
               delay_ms(30);Menu=3;//glcd_fillScreen(OFF);
               glcd_init(ON);             //Inicializa la glcd
               glcd_fillScreen(OFF);      //Limpia la pantalla
               glcd_update();  
               LuzBlanca=10;LuzUV=10;
            }
            else
            {
               if(Flanco6 == 0)
               {
                  Flanco6 = 1;delay_ms(30);flag_toma=!flag_toma;
               }
            }
         }
         else
         {
            Flanco6 = 0;
         }
               
        
      Envio_Esclavos();
    }
//_--------------------------------------------------------------------------------------------------------------------------------------------------------       

//_--------------------------------------------------------------------------------------------------------------------------------------------------------
      if(Menu==3){ //Menu Principal.
            
      strcpy(Menus.Letra,"Menu");
      displayMenu(Menus.Letra,40,0,1,2);
      
      if(Opcion==1)   
         {strcpy(Menus.Letra,"Ajustes");
         displayMenu(Menus.Letra,0,19,1,1);}
      else
         {strcpy(Menus.Letra,"Ajustes");
         displayMenu(Menus.Letra,0,19,0,1);}
         
      if(Opcion==2)   
         {strcpy(Menus.Letra,"Clave");
         displayMenu(Menus.Letra,0,35,1,1);}
      else
         {strcpy(Menus.Letra,"Clave");
         displayMenu(Menus.Letra,0,35,0,1);}   
         
       if(Opcion==3)   
         {strcpy(Menus.Letra,"Mantenimiento");
         displayMenu(Menus.Letra,0,51,1,1);}
      else
         {strcpy(Menus.Letra,"Mantenimiento");
         displayMenu(Menus.Letra,0,51,0,1);}     
       glcd_update(); 
       if(w==0)
       {delay_ms(500);w=1;}
         
        
      if(UP)//Si oprime hacia arriba
      {  if(Flanco == 0) 
            {Flanco = 1;delay_ms(30);Opcion--;
            }
      }
         else
            {Flanco = 0;}
            
      if(DOWN)//Si oprime hacia abajo
      {  if(Flanco2 == 0) 
            {Flanco2 = 1;delay_ms(30);Opcion++;
            }     
      }
         else
            {Flanco2 = 0;}  
            
      if(RIGHT)//Si oprime SET
      {  if(Flanco1 == 0) 
            {Flanco1 = 1;delay_ms(500);Menu=Opcion+3;Opcion=1;glcd_fillscreen(OFF);unidad=1;
            }
      }
         else
            {Flanco1 = 0;}  
            
      if(LEFT)//Si oprime boton de Toma.
      {delay_ms(700);Menu=2;glcd_fillscreen(OFF);}
            
      if(Opcion>3)
      {Opcion=1;}
      if(Opcion<1)
      {Opcion=3;}
    
    }
//_--------------------------------------------------------------------------------------------------------------------------------------------------------       

//_--------------------------------------------------------------------------------------------------------------------------------------------------------
      if(Menu==4){ //Menu de Ajustes.
      
      if(Opcion==1)   
         {strcpy(Menus.Letra,"Tiempo Purga");
         displayMenu(Menus.Letra,0,12,1,1);}
      else
         {strcpy(Menus.Letra,"Tiempo Purga");
         displayMenu(Menus.Letra,0,12,0,1);}
                           
      if(Opcion==2)   
         {strcpy(Menus.Letra,"Sensor de Flujo");
         displayMenu(Menus.Letra,0,24,1,1);}
      else
         {strcpy(Menus.Letra,"Sensor de Flujo");
         displayMenu(Menus.Letra,0,24,0,1);} 
         
      if(Opcion==3)   
         {strcpy(Menus.Letra,"Sensor de Presion");
         displayMenu(Menus.Letra,0,36,1,1);}
      else
         {strcpy(Menus.Letra,"Sensor de Presion");
         displayMenu(Menus.Letra,0,36,0,1);}          
         
        
      if(UP)//Si oprime hacia arriba
      {  if(Flanco == 0) 
            {Flanco = 1;delay_ms(30);Opcion--;
            }
      }
         else
            {Flanco = 0;}
            
      if(DOWN)//Si oprime hacia abajo
      {  if(Flanco2 == 0) 
            {Flanco2 = 1;delay_ms(30);Opcion++;
            }     
      }
         else
            {Flanco2 = 0;}  
            
      if(RIGHT)//Si oprime SET
      {  if(Flanco1 == 0) 
            {Flanco1 = 1;delay_ms(500);Menu=Opcion+10;glcd_fillScreen(OFF);
            }
      }
         else
            {Flanco1 = 0;}  
            
      if(LEFT)//Si oprime boton de Toma.
        {delay_ms(700);Menu=3;glcd_fillscreen(OFF);}
      
      if(Opcion>3)
      {Opcion=1;}
      if(Opcion<1)
      {Opcion=3;}
    
    }
//_--------------------------------------------------------------------------------------------------------------------------------------------------------       


//_--------------------------------------------------------------------------------------------------------------------------------------------------------
      if(Menu==11){ //Menu de Purga.
      
      strcpy(Menus.Letra,"T. Purga");
      displayMenu(Menus.Letra,10,0,1,2);
      
      if(unidad==4)
      {displayTiempo(Tpurgap[3],20,40,1,2);}
      else
      {displayTiempo(Tpurgap[3],20,40,0,2);}
      
      if(unidad==3)
      {displayTiempo(Tpurgap[2],40,40,1,2);}
      else
      {displayTiempo(Tpurgap[2],40,40,0,2);}
      
      if(unidad==2)
      {displayTiempo(Tpurgap[1],70,40,1,2);}
      else
      {displayTiempo(Tpurgap[1],70,40,0,2);}
      
      if(unidad==1)
      {displayTiempo(Tpurgap[0],90,40,1,2);}
      else
      {displayTiempo(Tpurgap[0],90,40,0,2);}
      
      glcd_text57(60, 42, Dos_puntos, 2, ON);
         
        
      if(UP)//Si oprime hacia arriba
      {  if(Flanco == 0) 
            {Flanco = 1;delay_ms(30);Tpurgap[unidad-1]++;
            }
      }
         else
            {Flanco = 0;}
            
      if(DOWN)//Si oprime hacia abajo
      {  if(Flanco2 == 0) 
            {Flanco2 = 1;delay_ms(30);Tpurgap[unidad-1]--;
            }     
      }
         else
            {Flanco2 = 0;}  
            
      if(RIGHT)//Si oprime SET
      {  if(Flanco1 == 0) 
            {Flanco1 = 1;delay_ms(30);unidad--;
            }
      }
         else
            {Flanco1 = 0;}  
            
      if(LEFT)//Si oprime boton de Toma.
        {delay_ms(700);Menu=3;glcd_fillscreen(OFF);Opcion=1;
         write_eeprom(4,Tpurgap[0]);write_eeprom(5,Tpurgap[1]);write_eeprom(6,Tpurgap[2]);write_eeprom(7,Tpurgap[3]);}
      
      for(i=0;i<4;i++)
      {
         if(Tpurgap[i]>9)
         {Tpurgap[i]=0;}
         
         if(Tpurgap[i]<0)
         {Tpurgap[i]=9;}
      }      
      for(i=1;i<4;i++)
      {
         if(Tpurgap[i]>5)
         {Tpurgap[i]=0;}
         
         if(Tpurgap[i]<0)
         {Tpurgap[i]=5;}
        i++; 
      }
      if(unidad<1)
      {unidad=4;}
      if(Opcion<1)
      {Opcion=4;}
    
    }
//_--------------------------------------------------------------------------------------------------------------------------------------------------------       

//_--------------------------------------------------------------------------------------------------------------------------------------------------------
      if(Menu==12){ //Menu de Sensor de Flujo.
      
      if(Ajuste1>10.0)
         Ajuste1=0.0;
      
      strcpy(Menus.Letra,"Sensor de Flujo");
      displayMenu(Menus.Letra,10,0,1,1);
      
      glcd_text57(0, 12, ZF, 1, ON);
      
      glcd_text57(0, 20, ZA, 1, ON);
      
      glcd_text57(0, 30, AJ, 1, ON);           
      
      displayfloat(zero_actual,44,20,0,1);
      displayfloat(Ajuste1,44,30,0,1);
      displayfloat(Inflow_adc,0,40,0,1);
      displayfloat(Diferencia,0,50,0,1);
      
      if(UP)//Si oprime hacia arriba
      {  
         delay_ms(30);Ajuste1+=0.01;
      }
            
      if(DOWN)//Si oprime hacia abajo
      {   
         delay_ms(30);Ajuste1-=0.01;               
      }
            
      if(RIGHT)//Si oprime SET
      {  
         zero_actual=Inflow_adc;
         Diferencia=zero_fabrica-Inflow_adc;
            
         if(Diferencia>=0)
         {
            negativo=10;write_eeprom(50,negativo);
         }
         if(Diferencia<0)
         {
            negativo=20;write_eeprom(50,negativo);
         }
         Diferencia=abs(Diferencia);
         Temporal1=(int16)Diferencia;
         write_eeprom(60,make8(Temporal1,0));
         write_eeprom(61,make8(Temporal1,1));//Guardar valor de Setpoint en eeprom
         
         Temporal=zero_actual;
         Entero=(int)zero_actual;
         Temporal=Temporal-Entero;
         Temporal2=Temporal*100.0;
         Decimal1=(int8)Temporal2;
         write_eeprom(29,Entero);write_eeprom(30,Decimal1);
      }
         else
            {Flanco1 = 0;}  
            
      if(LEFT)//Si oprime boton de Toma.
      {
         delay_ms(700);Menu=3;glcd_fillscreen(OFF);Opcion=1;
         Temporal=Ajuste1;
         Entero=(int)Ajuste1;
         Temporal=Temporal-Entero;
         Temporal2=Temporal*100.0;
         Decimal1=(int8)Temporal2;
         write_eeprom(25,Entero);write_eeprom(26,Decimal1);
      }
      
      Lectura_Esclavos();
    }
//_--------------------------------------------------------------------------------------------------------------------------------------------------------       

//_--------------------------------------------------------------------------------------------------------------------------------------------------------
      if(Menu==13){ //Menu de Sensor de Presión.
      
      if(Ajuste2>10.0)
         Ajuste2=0.0;
      
      strcpy(Menus.Letra,"Sensor de Presion");
      displayMenu(Menus.Letra,10,0,1,1);
      
      glcd_text57(0, 12, ZFP, 1, ON);
      
      glcd_text57(0, 20, ZA, 1, ON);
      
      glcd_text57(0, 30, AJ, 1, ON);           
      
      displayfloat(G,44,20,0,1);
      displayfloat(Ajuste2,44,30,0,1);
      displayfloat(Filtro_DownFlow_adc,0,40,0,1);
      displayfloat(Filtro_DownFlow,0,50,0,1);
      
      if(UP)//Si oprime hacia arriba
      {  
         delay_ms(30);Ajuste2+=0.01;
      }
            
      if(DOWN)//Si oprime hacia abajo
      {  
         delay_ms(30);Ajuste2-=0.01;
      }
            
      if(RIGHT)//Si oprime SET
      {  
         G=Filtro_Downflow_adc;
         G16=(int16)G;
         G_l=G16; G_h=(G16>>8);
            
         write_eeprom(62,G_l);write_eeprom(63,G_h);//Guardar valor de Setpoint en eeprom
      }
         else
            {Flanco1 = 0;}  
            
      if(LEFT)//Si oprime boton de Toma.
      {         
         delay_ms(700);Menu=3;glcd_fillscreen(OFF);Opcion=1;
         Temporal=Ajuste2;
         Entero=(int)Ajuste2;
         Temporal=Temporal-Entero;
         Temporal2=Temporal*100.0;
         Decimal1=(int8)Temporal2;
         write_eeprom(27,Entero);write_eeprom(28,Decimal1);
      }
      
      Lectura_Esclavos();
    }
//_--------------------------------------------------------------------------------------------------------------------------------------------------------       

//_--------------------------------------------------------------------------------------------------------------------------------------------------------
      if(Menu==5){ //Menu de Clave.
      
      
      displayContrasena(Contrasena[0],20,30,1);displayContrasena(Contrasena[1],40,30,0);
      displayContrasena(Contrasena[2],60,30,0);displayContrasena(Contrasena[0],80,30,0);
      strcpy(Menus.Letra,"Clave");
      displayMenu(Menus.Letra,20,0,1,3);
        
      if(UP)//Si oprime hacia arriba
      {  if(Flanco == 0) 
            {Flanco = 1;delay_ms(30);
            for(i=1;i<=4;i++)
               {
                  if(unidad==i)
                  {Contrasena[i-1]++;
                  if(Contrasena[i-1]>9)
                        {Contrasena[i-1]=0;}
                  displayContrasena(Contrasena[i-1],i*20,30,1);}
                  else
                  {displayContrasena(Contrasena[i-1],i*20,30,0);}
               }
            }
      }
         else
            {Flanco = 0;}
            
      if(DOWN)//Si oprime hacia abajo
      {  if(Flanco2 == 0) 
            {Flanco2 = 1;delay_ms(30);
               for(i=1;i<=4;i++)
               {
                  if(unidad==i)
                  {Contrasena[i-1]--; 
                     if(Contrasena[i-1]<0)
                        {Contrasena[i-1]=9;}
                   displayContrasena(Contrasena[i-1],i*20,30,1);}
                  else
                  {displayContrasena(Contrasena[i-1],i*20,30,0);}
               }      
            }     
      }
         else
            {Flanco2 = 0;}  
            
      if(RIGHT)//Si oprime SET
      {  if(Flanco1 == 0) 
            {Flanco1 = 1;delay_ms(30);unidad++;
               for(i=1;i<=4;i++)
               {
                  if(unidad==i)
                  {displayContrasena(Contrasena[i-1],i*20,30,1);}
                  else
                  {displayContrasena(Contrasena[i-1],i*20,30,0);}
               }
            }
      }
         else
            {Flanco1 = 0;}  
            
      if(LEFT)//Si oprime boton de Toma.
      {
         delay_ms(700);
         if(unidad==1)
         {
            Menu=3;glcd_fillscreen(OFF);
         }
         else
         {
            Menu=3;glcd_fillscreen(OFF);
         }
      }
            
      if(unidad>4)
      {Opcion=1;glcd_fillscreen(OFF);unidad=1;n=0;
         if((Contrasena[0]==Password[0])&&(Contrasena[1]==Password[1])&&(Contrasena[2]==Password[2])&&(Contrasena[3]==Password[3]))
         {glcd_imagen(1);glcd_update();Menu=211;delay_ms(1000);glcd_fillScreen(OFF);Password[0]=0;Password[1]=0;Password[2]=0;Password[3]=0;}
         else
         {glcd_imagen(2);glcd_update();Menu=3;delay_ms(1000);Contrasena[0]=0;Contrasena[1]=0;Contrasena[2]=0;Contrasena[3]=0;glcd_fillScreen(OFF);}  
      }
    
    }
//_--------------------------------------------------------------------------------------------------------------------------------------------------------       

//_--------------------------------------------------------------------------------------------------------------------------------------------------------
      if(Menu==211){ //Menu de Cambio Clave.
      
      if(n==0)
      {displayContrasena(Password[0],20,30,1);displayContrasena(Password[1],40,30,0);
      displayContrasena(Password[2],60,30,0);displayContrasena(Password[3],80,30,0);n++;}
      strcpy(Menus.Letra,"Clave Nueva");
      displayMenu(Menus.Letra,0,0,1,2);
        
      if(UP)//Si oprime hacia arriba
      {  if(Flanco == 0) 
            {Flanco = 1;delay_ms(30);
            for(i=1;i<=4;i++)
               {
                  if(unidad==i)
                  {Password[i-1]++;
                  if(Password[i-1]>9)
                        {Password[i-1]=0;}
                  displayContrasena(Password[i-1],i*20,30,1);}
                  else
                  {displayContrasena(Password[i-1],i*20,30,0);}
               }
            }
      }
         else
            {Flanco = 0;}
            
      if(DOWN)//Si oprime hacia abajo
      {  if(Flanco2 == 0) 
            {Flanco2 = 1;delay_ms(30);
               for(i=1;i<=4;i++)
               {
                  if(unidad==i)
                  {Password[i-1]--; 
                     if(Password[i-1]<0)
                        {Password[i-1]=9;}
                   displayContrasena(Password[i-1],i*20,30,1);}
                  else
                  {displayContrasena(Password[i-1],i*20,30,0);}
               }      
            }     
      }
         else
            {Flanco2 = 0;}  
            
      if(RIGHT)//Si oprime SET
      {  if(Flanco1 == 0) 
            {Flanco1 = 1;delay_ms(30);unidad++;
               for(i=1;i<=4;i++)
               {
                  if(unidad==i)
                  {displayContrasena(Password[i-1],i*20,30,1);}
                  else
                  {displayContrasena(Password[i-1],i*20,30,0);}
               }
            }
      }
         else
            {Flanco1 = 0;}  
      if(unidad>4)
      {Opcion=1;glcd_fillscreen(OFF);unidad=1;n=0;
      write_eeprom(0,Password[0]);write_eeprom(1,Password[1]);write_eeprom(2,Password[2]);write_eeprom(3,Password[3]);
       glcd_imagen(1);glcd_update();Menu=211;delay_ms(1000);glcd_fillScreen(OFF);Menu=3;
      }
    
    }
//_--------------------------------------------------------------------------------------------------------------------------------------------------------       

//_--------------------------------------------------------------------------------------------------------------------------------------------------------
      if(Menu==6){ //Menu de Mantenimiento.
      
         glcd_text57(0, 29, Vida_Filtro, 1, ON);
         displayTiempo(Ttrabajo[3],60,28,0,1);displayTiempo(Ttrabajo[2],67,28,0,1);displayTiempo(Ttrabajo[1],74,28,0,1);displayTiempo(Ttrabajo[0],82,28,0,1);
         LuzBlanca=5;
         
            if(DOWN) // Reinicia Tiempo de Trabajo
            {
               delay_ms(2000);
                  write_eeprom(16,0);write_eeprom(17,0);write_eeprom(18,0);write_eeprom(19,0);
                  Ttrabajo[3]=0;Ttrabajo[2]=0;Ttrabajo[1]=0;Ttrabajo[0]=0;
                  Menu=3;glcd_fillscreen(OFF);LuzBlanca=10;
            }
            
            if(LEFT)
            {
               Menu=3;glcd_fillscreen(OFF);LuzBlanca=10;
            }
    }
//_--------------------------------------------------------------------------------------------------------------------------------------------------------       


//_--------------------------------------------------------------------------------------------------------------------------------------------------------
      if(Menu==200){ //Menu de Inicio.  
         Display_on;
         glcd_init(ON);             //Inicializa la glcd
         glcd_fillScreen(OFF);      //Limpia la pantalla
         glcd_text57(0, 0, JP, 2, ON);glcd_text57(5, 20, Cabina, 1, ON);glcd_text57(10, 40, Clase, 2, ON);             
         glcd_update();   
         delay_ms(3000);
                    
         glcd_fillScreen(OFF);      //Limpia la pantalla
         displayContrasena(Contrasena[0],20,30,1);displayContrasena(Contrasena[1],40,30,0);
         displayContrasena(Contrasena[2],60,30,0);displayContrasena(Contrasena[0],80,30,0);
         glcd_rect(0, 0, 127, 25, YES, ON);strcpy(Menus.Letra,"Clave");glcd_text57(25, 1, Menus.Letra, 3, OFF);   
         Menu=0;
      Envio_Esclavos();
    }
//_--------------------------------------------------------------------------------------------------------------------------------------------------------       

   if(tiemporeset>=60)
   {
      glcd_init(ON);
      tiemporeset=0;
   } 
}

}


