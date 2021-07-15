//Programa para Cabina de Bioseguridad

#include <18F4685.h>
#device adc=10
#fuses HS,NOWDT,NOPROTECT,NOLVP,NODEBUG,NOSTVREN,NOPUT,NOCPD,NOWRT,NOIESO,NOFCMEN,NOPBADEN,NOWRTC,NOWRTB,NOEBTR,NOEBTRB,NOCPB,NOXINST,,NOLPT1OSC,NOMCLR
#use delay(clock=20000000)
#include <MATH.h>
#include "HDM64GS12.c"
#include "GRAPHICS.c"
#include "imagen.h"
#include "FuncionesDisplay.h"

#define   UP               input(PIN_E1)
#define   DOWN             input(PIN_E2)
#define   RIGHT            input(PIN_C0)
#define   LEFT             input(PIN_C1)
#define   VIDRIOUP         input(PIN_A5)
#define   VIDRIODN         input(PIN_A4)

#define   Alarma_on           output_bit(PIN_C2,1)
#define   Alarma_off          output_bit(PIN_C2,0)
#define   LuzBlanca_on        output_bit(PIN_C4,1)
#define   LuzBlanca_off       output_bit(PIN_C4,0)
#define   LuzUV_on            output_bit(PIN_C5,1)
#define   LuzUV_off           output_bit(PIN_C5,0)
#define   Motor_on            output_bit(PIN_C6,1)
#define   Motor_off           output_bit(PIN_C6,0)
#define   Toma_on             output_bit(PIN_C7,1)
#define   Toma_off            output_bit(PIN_C7,0)
#define   Display_on          output_bit(PIN_B1,1)
#define   Display_off         output_bit(PIN_B1,0)
#define   O1_on               output_bit(PIN_E0,1)
#define   O1_off              output_bit(PIN_E0,0)
#define   O2_on               output_bit(PIN_C3,1)
#define   O2_off              output_bit(PIN_C3,0)

int16 tiempos=0;
int8 i=0;

short Flanco=0,Flanco1=0,Flanco2=0,Flanco3=0,Flanco4=0,Flanco5=0,Flanco6=0,imprimir=0,flag_uv=0,flag_blower=0,flag_toma=0,flag_luz=0,flag_alarma=0,w=0;
short tiempo_purga=0,tiempo_uv=0,tiempo_trabajo=0,tiempo_postpurga=0,Latencia=0,flag_latencia=0;
short guardauv=0,guardatrabajo=0,Lectura=0,n=0,flag_alarma2=0;
char  JP[] = "JP Inglobal", Cabina[] = "Cabina Extractora",tiempo_est[]="T est=", Dos_puntos[]=":";
char  Clase[] = "Sin Ductos", Tiempo[] = "Tiempo de Purga", UV[]="UV";
char  Baje_Vidrio[] = "!!Ubique Vidrio!!",Inflowt[] = "JPCEGH-F-A",Downflowt[] = "Pres(inH2O):", ZF[] = "Fabrica:208.0", ZA[] = "Actual:",AJ[]="Ajuste:";
char  Vida_UV[] = "UVLife:",Vida_Filtro[] = "Filter:",ZFP[] = "Fabrica:235.0", Perdida[]="!Filtro Saturado!";
int8 Menu=200, unidad=1, Opcion=1,inicie=0,negativo=10,G_l=0,G_h=0,tiemporeset=0;
int8 q=0,r=0,l=0,h=0,Entero=0,Decimal1=0,LuzUV=10,Motor=10,Alarma=10,UV2=0,e=0,m=0;

unsigned int16 Inflow_adc=0,Filtro_Downflow_adc=0,Temporal1=0;

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
float Inflow=0.0,Downflow=0.0,Filtro_Downflow=0.0,Diferencia=0.0,barra=0.0,barra_old=0.0,barra2=0.0,barra_old2=0.0;
float Velocidad=0.0,x_uno=0.0,x_cuadrado=0.0,x_cubo=0.0,Ajuste1=0.0,Ajuste2=0.0;
float ADC=0,Temporal=0.0,Temporal2=0.0;
float a=0.00000957915546738639,b=-0.00727819571978831,c= 1.85375754678451,d= -156.816806902063;
//float t=378.463968402560,f=-18275.0402762787;//Sensor 2 Posible bueno

float promedio[8]={0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
float PromPresion[8]={0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
float Flujo=0.0,zero_fabrica=208.0,zero_actual=0.0;
float V1=0.0,Presion=0.0,G=2.0,UVTime=0.0;

int16 Vector[40];

int1 BITS[32];
int16 Ta,Tb,TICK=0;
Int1 BIT_START, NUEVO_DATO=0;
int BYTE_IR[4];
int8 BIT_BYTE(int1 D7, int1 D6, int1 D5, int1 D4, int1 D3, int1 D2, int1 D1, int1 D0);

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


Float LeerFlujoInFlow(int media)
{
    InFlow_adc=sensores(0);
      
    if(negativo==10)
    {
      x_uno=Inflow_adc+Diferencia;
    }
    if(negativo==20)
    {
      x_uno=Inflow_adc-Diferencia;
    }
    Velocidad=(pow(x_uno,3)*a)+(pow(x_uno,2)*b)+(x_uno*c)+(d);
    //Velocidad=Velocidad*correccion;//Ajuste de Temperatura
    
    if(Velocidad<0.0)
     {Velocidad=0.0;}
     
     if(l>=media)
       {l=0;}
     
     promedio[l]=Velocidad;l++;
     Flujo=0;
        for(h=0;h<=(media-1);h++)
        {
           Flujo+=promedio[h];
        } 
           Flujo=(Flujo/media)*Ajuste1; 
           return Flujo;
}

float Leer_Sensor_Presion(int media){
   float promediopresion=0.0;
   
   Filtro_Downflow_adc=sensores(2);
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
   set_timer1(40536);
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
      
      if(Alarma==5)
      {
         if(flag_alarma2==0)
         {
            Alarma_on;
            flag_alarma2=1;
         }
         else
         {
            Alarma_off;
            flag_alarma2=0;
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

void main(){

   setup_timer_1(T1_INTERNAL|T1_DIV_BY_1);    
   enable_interrupts(INT_TIMER1);
   setup_adc_ports(AN0_TO_AN3);
   setup_adc(ADC_CLOCK_DIV_32 );
   enable_interrupts(global);   
   
   Password[0]=read_eeprom(0);
   delay_ms(10);
   Password[1]=read_eeprom(1);
   delay_ms(10);
   Password[2]=read_eeprom(2);
   delay_ms(10);
   Password[3]=read_eeprom(3);
   delay_ms(10);
   Tpurgap[0]=read_eeprom(4);
   delay_ms(10);
   Tpurgap[1]=read_eeprom(5);
   delay_ms(10);
   Tpurgap[2]=read_eeprom(6);
   delay_ms(10);
   Tpurgap[3]=read_eeprom(7);
   delay_ms(10);
   Tppurgap[0]=read_eeprom(8);
   delay_ms(10);
   Tppurgap[1]=read_eeprom(9);
   delay_ms(10);
   Tppurgap[2]=read_eeprom(10);
   delay_ms(10);
   Tppurgap[3]=read_eeprom(11);
   delay_ms(10);
   Tuv[0]=read_eeprom(12);
   delay_ms(10);
   Tuv[1]=read_eeprom(13);
   delay_ms(10);
   Tuv[2]=read_eeprom(14);
   delay_ms(10);
   Tuv[3]=read_eeprom(15);
   delay_ms(10);
   Ttrabajo[0]=read_eeprom(16);
   delay_ms(10);
   Ttrabajo[1]=read_eeprom(17);
   delay_ms(10);
   Ttrabajo[2]=read_eeprom(18);
   delay_ms(10);
   Ttrabajo[3]=read_eeprom(19);
   delay_ms(10);
   flag_alarma=read_eeprom(20);
    
   delay_ms(10);
   Entero=read_eeprom(25);//Decimal1=read_eeprom(26);
   delay_ms(10);
   Temporal=read_eeprom(26);
   delay_ms(10);
   Temporal=Temporal/100.0;
   Ajuste1=Entero+Temporal;
   
   
   Entero=read_eeprom(27);//Decimal1=read_eeprom(26);
   delay_ms(10);
   Temporal=read_eeprom(28);
   delay_ms(10);
   Temporal=Temporal/100.0;
   Ajuste2=Entero+Temporal;
   
   
   Entero=read_eeprom(29);//Decimal1=read_eeprom(26);
   delay_ms(10);
   zero_actual=Entero;
   
   Diferencia=make16(read_eeprom(61),read_eeprom(60));
   delay_ms(10);
   G=make16(read_eeprom(63),read_eeprom(62));
   delay_ms(10);
   negativo=read_eeprom(50);
   delay_ms(10);
   Menu=200;

   if(G>1000)G=240;
   if(Ajuste1<=0.0 || Ajuste1>5.0)Ajuste1=1.0;
   if(Ajuste2<=0.0 || Ajuste2>5.0)Ajuste2=1.0;
   while(true){
   
   if(NUEVO_DATO==1){
         NUEVO_DATO=0;
         BYTE_IR[0]=BIT_BYTE(BITS[0],BITS[1],BITS[2],BITS[3],BITS[4],BITS[5],BITS[6],BITS[7]);
         BYTE_IR[1]=BIT_BYTE(BITS[8],BITS[9],BITS[10],BITS[11],BITS[12],BITS[13],BITS[14],BITS[15]);
         BYTE_IR[2]=BIT_BYTE(BITS[16],BITS[17],BITS[18],BITS[19],BITS[20],BITS[21],BITS[22],BITS[23]);
         BYTE_IR[3]=BIT_BYTE(BITS[24],BITS[25],BITS[26],BITS[27],BITS[28],BITS[29],BITS[30],BITS[31]);
         
         delay_ms(500);
      }

   if(BYTE_IR[3]==0XF2 && Menu!=200) //Subir Vidrio
   {
      if(!VIDRIOUP)
         O1_on;O2_off;
   }
   
   if(BYTE_IR[3]==0XE9 && Menu!=200)
   {
      if(!VIDRIODN)
         O1_off;O2_on;
   }
  
   if(BYTE_IR[3]==0XE6)
   {
      O1_off;O2_off;
   }
   
   glcd_update();
   
   if(Lectura==1)
   {
      Lectura=0;
      InFlow=LeerFlujoInFlow(8);
      Filtro_DownFlow=Leer_Sensor_Presion(8);
   }
   
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
   {
      Alarma_off;
      Alarma=10;
   }
//_--------------------------------------------------------------------------------------------------------------------------------------------------------
      if(Menu==0){ //Menu de Contraseña.
      flag_latencia=1;
      if(Latencia==1)
      {
         glcd_fillScreen(OFF);glcd_init(OFF);Display_off;Menu=200;reset_cpu();
      }
      
      if(UP || BYTE_IR[2]==0X18)//Si oprime hacia arriba
      {
         flag_latencia=0;t_latencia=0;BYTE_IR[2]=0X00;
         if(Flanco == 0)
         {
            Flanco = 1;delay_ms(5);
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
            
      if(DOWN || BYTE_IR[2]==0X52)//Si oprime hacia abajo
      {
         flag_latencia=0;t_latencia=0;BYTE_IR[2]=0X00;
         if(Flanco2 == 0)
         {
            Flanco2 = 1;delay_ms(5);
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
            
      if(RIGHT || BYTE_IR[2]==0X5A)//Si oprime SET
      {
         flag_latencia=0;t_latencia=0;BYTE_IR[2]=0X00;
         if(Flanco1 == 0)
         {
            Flanco1 = 1;delay_ms(5);unidad++;
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
         tiempo_purga=1;LuzBlanca_off;LuzUV_off;Toma_off;Alarma_off;Motor_on;
         glcd_rect(9, 54, 120, 64, YES, OFF);         
         displayTiempo(Tpurga[3],20,20,0,2);displayTiempo(Tpurga[2],40,20,0,2);glcd_text57(60, 22, Dos_puntos, 2, ON);
         displayTiempo(Tpurga[1],70,20,0,2);displayTiempo(Tpurga[0],90,20,0,2);        
      
         if(RIGHT || BYTE_IR[2]==0X1C)
         {
            delay_ms(1000);
            if(RIGHT || BYTE_IR[2]==0X1C)
            {
               tiempo_purga=0;Menu=2;glcd_fillScreen(OFF);BYTE_IR[2]=0X00;
            }
         }
       
         if((Tpurga[0]>=Tpurgap[0])&&(Tpurga[1]>=Tpurgap[1])&&(Tpurga[2]>=Tpurgap[2])&&(Tpurga[3]>=Tpurgap[3]))
         {
            tiempo_purga=0;Menu=2;glcd_fillScreen(OFF);flag_luz=!flag_luz;flag_blower=1;flag_toma=1;
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
            displayfloat(Filtro_Downflow,70,21,0,1);   
            imprimir=0;
         }
         
         if(flag_blower==1)
         {
            glcd_imagen(5);tiempo_trabajo=1;Motor_on;
            if(guardatrabajo==1)
            {
               guardatrabajo=0;write_eeprom(30,minutos_trabajo);delay_ms(2);
               write_eeprom(16,Ttrabajo[0]);delay_ms(2);
               write_eeprom(17,Ttrabajo[1]);delay_ms(2);
               write_eeprom(18,Ttrabajo[2]);delay_ms(2);
               write_eeprom(19,Ttrabajo[3]);delay_ms(2);
               glcd_init(ON);
            }
         }
         else
         {
            Filtro_Downflow=0;
            glcd_rect(102, 45, 127, 64, YES, OFF);tiempo_trabajo=0;Motor_off;
            if(guardatrabajo==0)
            {
               guardatrabajo=1;write_eeprom(30,minutos_trabajo);delay_ms(2);
               write_eeprom(16,Ttrabajo[0]);delay_ms(2);
               write_eeprom(17,Ttrabajo[1]);delay_ms(2);
               write_eeprom(18,Ttrabajo[2]);delay_ms(2);
               write_eeprom(19,Ttrabajo[3]);delay_ms(2);
               glcd_init(ON);
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

         if(Filtro_Downflow>1.5)
         {
            flag_latencia=1;
            if(Latencia==1)
            {         
               if(Filtro_Downflow>1.5)
               {
                  Alarma=5;
               }
            }
         }
         else
         {
            Latencia=0;t_latencia=0;flag_latencia=0;
            glcd_rect(0, 54, 104, 64, YES, OFF);Alarma_off;Alarma=10;
         }
         
         if(flag_luz==1)
         {
            glcd_imagen(3);LuzBlanca_on;
         }
         else
         {
            glcd_rect(102, 14, 127, 28, YES, OFF);LuzBlanca_off;
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
         
            
      if(RIGHT || BYTE_IR[2]==0X5A)//Si oprime boton de Blower.
      {
         if(Flanco3 == 0)
         {
            Flanco3 = 1;delay_ms(5);flag_blower=!flag_blower;BYTE_IR[2]=0X00;
         }
      }
      else
      {
         Flanco3 = 0;
      }
            
      if(DOWN || BYTE_IR[2]==0X52)//Si oprime boton de Luz Blanca.
      {
         if(Flanco5 == 0)
         {
            Flanco5 = 1;delay_ms(5);flag_luz=!flag_luz;BYTE_IR[2]=0X00;
         }
      }
      else
      {
         Flanco5 = 0;
      }
      
      if(BYTE_IR[2]==0X1C)
      {
         BYTE_IR[2]=0X00;
         delay_ms(30);Menu=3;//glcd_fillScreen(OFF);
         glcd_init(ON);             //Inicializa la glcd
         glcd_fillScreen(OFF);      //Limpia la pantalla
         glcd_update();  
         LuzBlanca_off;LuzUV_off;
      }
      
      if(BYTE_IR[2]==0X09)
      {
         BYTE_IR[2]=0X00;
         delay_ms(30);Menu=32;//glcd_fillScreen(OFF);
         glcd_init(ON);             //Inicializa la glcd
         glcd_fillScreen(OFF);      //Limpia la pantalla
         glcd_update();  
         LuzBlanca_off;LuzUV_off;
      }
      
      if(UP || BYTE_IR[2]==0X18)//Si oprime boton de Toma.
      {
         BYTE_IR[2]=0X00;
         delay_ms(200);
         if(UP)//Si oprime boton de Toma.
         {
            delay_ms(30);Menu=3;//glcd_fillScreen(OFF);
            glcd_init(ON);             //Inicializa la glcd
            glcd_fillScreen(OFF);      //Limpia la pantalla
            glcd_update();  
            LuzBlanca_off;LuzUV_off;
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
         
        
      if(UP || BYTE_IR[2]==0X18)//Si oprime hacia arriba
      {  
         if(Flanco == 0)
         {
            Flanco = 1;delay_ms(30);Opcion--;BYTE_IR[2]=0X00;
         }
      }
      else
      {
         Flanco = 0;
      }
            
      if(DOWN  || BYTE_IR[2]==0X52)//Si oprime hacia abajo
      {  if(Flanco2 == 0) 
            {Flanco2 = 1;delay_ms(30);Opcion++;BYTE_IR[2]=0X00;
            }     
      }
         else
            {Flanco2 = 0;}  
            
      if(RIGHT || BYTE_IR[2]==0X5A)//Si oprime SET
      {  if(Flanco1 == 0) 
            {Flanco1 = 1;delay_ms(500);Menu=Opcion+3;Opcion=1;glcd_fillscreen(OFF);unidad=1;BYTE_IR[2]=0X00;
            }
      }
         else
            {Flanco1 = 0;}  
            
      if(LEFT || BYTE_IR[2]==0X08)//Si oprime boton de Toma.
      {delay_ms(700);Menu=2;glcd_fillscreen(OFF);BYTE_IR[2]=0X00;}
            
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
         {strcpy(Menus.Letra,"Sensor de Presion");
         displayMenu(Menus.Letra,0,36,1,1);}
      else
         {strcpy(Menus.Letra,"Sensor de Presion");
         displayMenu(Menus.Letra,0,36,0,1);}        
         
        
      if(UP || BYTE_IR[2]==0X18)//Si oprime hacia arriba
      {  if(Flanco == 0) 
            {Flanco = 1;delay_ms(30);Opcion--;BYTE_IR[2]=0X00;
            }
      }
         else
            {Flanco = 0;}
            
      if(DOWN  || BYTE_IR[2]==0X52)//Si oprime hacia abajo
      {  if(Flanco2 == 0) 
            {Flanco2 = 1;delay_ms(30);Opcion++;BYTE_IR[2]=0X00;
            }     
      }
         else
            {Flanco2 = 0;}  
            
      if(RIGHT  || BYTE_IR[2]==0X5A)//Si oprime SET
      {  if(Flanco1 == 0) 
            {Flanco1 = 1;delay_ms(500);Menu=Opcion+10;glcd_fillScreen(OFF);BYTE_IR[2]=0X00;
            }
      }
         else
            {Flanco1 = 0;}  
            
      if(LEFT || BYTE_IR[2]==0X08)//Si oprime boton de Toma.
        {delay_ms(700);Menu=3;glcd_fillscreen(OFF);BYTE_IR[2]=0X00;}
      
      if(Opcion>2)
      {Opcion=1;}
      if(Opcion<1)
      {Opcion=2;}
    
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
         
        
      if(UP || BYTE_IR[2]==0X18)//Si oprime hacia arriba
      {  if(Flanco == 0) 
            {Flanco = 1;delay_ms(30);Tpurgap[unidad-1]++;BYTE_IR[2]=0X00;
            }
      }
         else
            {Flanco = 0;}
            
      if(DOWN  || BYTE_IR[2]==0X52)//Si oprime hacia abajo
      {  if(Flanco2 == 0) 
            {Flanco2 = 1;delay_ms(30);Tpurgap[unidad-1]--;BYTE_IR[2]=0X00;
            }     
      }
         else
            {Flanco2 = 0;}  
            
      if(RIGHT || BYTE_IR[2]==0X5A)//Si oprime SET
      {  if(Flanco1 == 0) 
            {Flanco1 = 1;delay_ms(30);unidad--;BYTE_IR[2]=0X00;
            }
      }
         else
            {Flanco1 = 0;}  
            
      if(LEFT || BYTE_IR[2]==0X08)//Si oprime boton de Toma.
        {delay_ms(700);Menu=3;glcd_fillscreen(OFF);Opcion=1;BYTE_IR[2]=0X00;
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
      if(Menu==12){ //Menu de Sensor de Presión.
      
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
      
      if(UP || BYTE_IR[2]==0X18)//Si oprime hacia arriba
      {  
         delay_ms(30);Ajuste2+=0.01;BYTE_IR[2]=0X00;
      }
            
      if(DOWN  || BYTE_IR[2]==0X52)//Si oprime hacia abajo
      {  
         delay_ms(30);Ajuste2-=0.01;BYTE_IR[2]=0X00;
      }
            
      if(RIGHT || BYTE_IR[2]==0X5A)//Si oprime SET
      {  
         BYTE_IR[2]=0X00;
         G=Filtro_Downflow_adc;
         G16=(int16)G;
         G_l=G16; G_h=(G16>>8);
            
         write_eeprom(62,G_l);write_eeprom(63,G_h);//Guardar valor de Setpoint en eeprom
      }
         else
            {Flanco1 = 0;}  
            
      if(LEFT || BYTE_IR[2]==0X08)//Si oprime boton de Toma.
      {         
         BYTE_IR[2]=0X00;
         delay_ms(700);Menu=3;glcd_fillscreen(OFF);Opcion=1;
         Temporal=Ajuste2;
         Entero=(int)Ajuste2;
         Temporal=Temporal-Entero;
         Temporal2=Temporal*100.0;
         Decimal1=(int8)Temporal2;
         write_eeprom(27,Entero);write_eeprom(28,Decimal1);
      }
    }
//_--------------------------------------------------------------------------------------------------------------------------------------------------------       

//_--------------------------------------------------------------------------------------------------------------------------------------------------------
      if(Menu==5){ //Menu de Clave.
      
      
      displayContrasena(Contrasena[0],20,30,1);displayContrasena(Contrasena[1],40,30,0);
      displayContrasena(Contrasena[2],60,30,0);displayContrasena(Contrasena[0],80,30,0);
      strcpy(Menus.Letra,"Clave");
      displayMenu(Menus.Letra,20,0,1,3);
        
      if(UP || BYTE_IR[2]==0X18)//Si oprime hacia arriba
      {  if(Flanco == 0) 
            {Flanco = 1;delay_ms(30);BYTE_IR[2]=0X00;
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
            
      if(DOWN  || BYTE_IR[2]==0X52)//Si oprime hacia abajo
      {  if(Flanco2 == 0) 
            {Flanco2 = 1;delay_ms(30);BYTE_IR[2]=0X00;
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
            
      if(RIGHT || BYTE_IR[2]==0X5A)//Si oprime SET
      {  if(Flanco1 == 0) 
            {Flanco1 = 1;delay_ms(30);unidad++;BYTE_IR[2]=0X00;
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
            
      if(LEFT || BYTE_IR[2]==0X08)//Si oprime boton de Toma.
      {
         BYTE_IR[2]=0X00;
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
        
      if(UP || BYTE_IR[2]==0X18)//Si oprime hacia arriba
      {  if(Flanco == 0) 
            {Flanco = 1;delay_ms(30);BYTE_IR[2]=0X00;
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
            
      if(DOWN  || BYTE_IR[2]==0X52)//Si oprime hacia abajo
      {  if(Flanco2 == 0) 
            {Flanco2 = 1;delay_ms(30);BYTE_IR[2]=0X00;
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
            
      if(RIGHT || BYTE_IR[2]==0X5A)//Si oprime SET
      {  if(Flanco1 == 0) 
            {Flanco1 = 1;delay_ms(30);unidad++;BYTE_IR[2]=0X00;
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
      
         strcpy(Menus.Letra,"Mantenimien");
         displayMenu(Menus.Letra,00,0,1,2);
         glcd_text57(0, 29, Vida_Filtro, 1, ON);
         displayTiempo(Ttrabajo[3],60,28,0,1);displayTiempo(Ttrabajo[2],67,28,0,1);displayTiempo(Ttrabajo[1],74,28,0,1);displayTiempo(Ttrabajo[0],82,28,0,1);
         LuzBlanca_on;
         
            
            if((UP && DOWN)  || BYTE_IR[2]==0X52) // Reinicia Tiempo de Trabajo
            {
                  BYTE_IR[2]=0X00;
                  delay_ms(2000);
                  write_eeprom(16,0);write_eeprom(17,0);write_eeprom(18,0);write_eeprom(19,0);
                  Ttrabajo[3]=0;Ttrabajo[2]=0;Ttrabajo[1]=0;Ttrabajo[0]=0;
                  Menu=3;glcd_fillscreen(OFF);LuzBlanca_off;
            }
            
            if(LEFT)
            {
               Menu=3;glcd_fillscreen(OFF);LuzBlanca_off;BYTE_IR[2]=0X00;
            }
    }
//_--------------------------------------------------------------------------------------------------------------------------------------------------------       

//_--------------------------------------------------------------------------------------------------------------------------------------------------------
      if(Menu==200){ //Menu de Inicio.
         
      
            BYTE_IR[2]=0X00;
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
            Motor_off;LuzBlanca_off;LuzUV_off;Toma_off;Alarma_off;
    }
//_--------------------------------------------------------------------------------------------------------------------------------------------------------       

   if(tiemporeset>=60)
   {
      if(Menu!=200){
         glcd_init(ON);
         tiemporeset=0;
      }
   } 
}

}
int8 BIT_BYTE(int1 D0, int1 D1, int1 D2, int1 D3, int1 D4, int1 D5, int1 D6, int1 D7){
   int8 dato;
   dato= D7*128 + D6*64 + D5*32 + D4*16 + D3*8 + D2*4 + D1*2 +D0*1; // 0 al 255 
   return dato; 
}

