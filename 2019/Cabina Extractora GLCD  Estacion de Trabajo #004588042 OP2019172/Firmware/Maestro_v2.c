//Programa para Cabina de Bioseguridad

#include <18F4685.h>
#device adc=10
#fuses HS,NOWDT,NOPROTECT,NOLVP,NODEBUG,NOSTVREN,NOPUT,NOCPD,NOWRT,NOIESO,NOFCMEN,NOPBADEN,NOWRTC,NOWRTB,NOEBTR,NOEBTRB,NOCPB,NOXINST,NOLPT1OSC,NOMCLR
#use delay(clock=20000000)
#include <MATH.h>
#include "HDM64GS12.c"
#include "GRAPHICS.c"
#include "imagen.h"
#include "FuncionesDisplay.h"

#define   UP               !input(PIN_A5)
#define   DOWN             !input(PIN_E0)
#define   RIGHT            !input(PIN_E1)
#define   LEFT             !input(PIN_E2)
#define   VIDRIOUP         input(PIN_A3)
#define   VIDRIODN         input(PIN_A4)

#define   Alarma_on           output_bit(PIN_C7,1)
#define   Alarma_off          output_bit(PIN_C7,0)
#define   LuzBlanca_on        output_bit(PIN_C0,1)
#define   LuzBlanca_off       output_bit(PIN_C0,0)
#define   Aux_on              output_bit(PIN_C1,1)
#define   Aux_off             output_bit(PIN_C1,0)
#define   Motor_on            output_bit(PIN_C2,1)
#define   Motor_off           output_bit(PIN_C2,0)
#define   Display_on          output_bit(PIN_B1,1)
#define   Display_off         output_bit(PIN_B1,0)

short Flanco=0,Flanco1=0,Flanco2=0,Flanco3=0,Flanco5=0,imprimir=0,flag_blower=0,flag_toma=0,flag_luz=0,flag_alarma=0,w=0;
short tiempo_purga=0,tiempo_trabajo=0,flag_latencia=0;
short guardatrabajo=0,Lectura=0,n=0;
char  JP[] = "JP Inglobal", Cabina[] = "   Estacion de",Dos_puntos[]=":";
char  Clase[] = " Trabajo", Tiempo[] = "Tiempo de Purga";
char  Inflowt[] = "Vel:",Downflowt[] = "Temperatura", ZF[] = "Fabrica:208.0", ZA[] = "Actual:",AJ[]="Ajuste:";
char  Vida_Filtro[] = "T.Trabajo:",ZFP[] = "Fabrica:235.0", Perdida[]="!Filtro Saturado!";
int8 Menu=200, unidad=1, Opcion=1,negativo=10,G_l=0,G_h=0,tiemporeset=0;
int8 q=0,r=0,l=0,h=0,Entero=0,Decimal1=0,Alarma=10;

unsigned int16 Inflow_adc=0,Filtro_Downflow_adc=0,Temporal1=0;

int16 t_latencia=0,minutos_uv=0,minutos_trabajo=0,G16=0;
signed int  Tpurga[4]={0,0,0,0};       // Tiempo de Purga transcurrido
signed int  Tpurgap[4]={9,0,0,0};      // Tiempo de Purga programado
signed int  Tppurgap[4]={9,0,0,0};     // Tiempo de Post-Purga programado
signed int  Tuv[4]={0,0,0,0};          // Tiempo de UV transcurrido(en Horas)
signed int  Tempouvp[4]={0,0,0,0};        // Tiempo de UV transcurrido
signed int  Ttrabajo[4]={0,0,0,0};        // Tiempo de trabajo transcurrido (en Horas)
signed int  Contrasena[4]={0,0,0,0};      // Contrasena de 4 digitos
signed int  Password[4]={0,0,0,0};        // Contrasena Almacenada de 4 digitos
float Inflow=0.0,Downflow=0.0,Filtro_Downflow=0.0,Diferencia=0.0;
float Velocidad=0.0,x_uno=0.0,x_cuadrado=0.0,x_cubo=0.0,Ajuste1=0.0,Ajuste2=0.0;
float Temporal=0.0,Temporal2=0.0;
float a=0.00000957915546738639,b=-0.00727819571978831,c= 1.85375754678451,d= -156.816806902063;
//float t=378.463968402560,f=-18275.0402762787;//Sensor 2 Posible bueno

float promedio[8]={0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
float promedio2[8]={0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
float PromPresion[8]={0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
float Flujo=0.0,zero_fabrica=208.0,zero_actual=0.0;
float V1=0.0,Presion=0.0,G=2.0;

int16 pulsos=0,retardo=0;
int8 i=0,l2=0,h2=0,Temperatura=0;
short Cruce=OFF,Purga=OFF;
int16 tiempocruce=0;
int8 Porcentaje=0,VelMotor=0;
float Tcruce=0.0,Tcruce2=0;

#include "Funciones.h"

#int_EXT            // Interrupción de encoder para control.
void ext0() {       // 6 pulsos por vuelta.
   pulsos++;
   if(pulsos>=120){
      pulsos=0;
      imprimir=1;Lectura=1;tiemporeset++;
   
      if(flag_blower==1)
         minutos_trabajo++; 
  
      if(tiempo_purga==1 || Purga){                  
         Tpurga[0]++;
         if(Tpurga[0]>9){
            Tpurga[0]=0;
            Tpurga[1]++;
         }
         if(Tpurga[1]>5){
            Tpurga[1]=0;
            Tpurga[2]++;
         }
         if(Tpurga[2]>9){
            Tpurga[2]=0;
            Tpurga[3]++;
         }
         if(Tpurga[3]>5){
            Tpurga[3]=0;
         }
      } 
   }
   tiempocruce=0;
   Cruce=ON;
   if(VelMotor<6){
      if(VelMotor>0 && Porcentaje<100){
         delay_us(retardo);      
         Motor_on;
         delay_us(100);      
      }
      Motor_off;
   }
}

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
    InFlow_adc=sensores(1);
      
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

void main(){
   delay_ms(1000);
   Cruce=OFF;
   VelocidadMotor(0);
   Tcruce2=83;
   Tcruce=8300;
   Motor_off;
   //setup_timer_1(T1_INTERNAL|T1_DIV_BY_1);
   //enable_interrupts(INT_TIMER1);
   enable_interrupts(INT_EXT_L2H);
   setup_adc_ports(AN0_TO_AN1);
   setup_adc(ADC_CLOCK_DIV_32 );
   enable_interrupts(global);  
   VelocidadMotor(0);
   Motor_off;
//-------Inicio de Cabina-------------------------------------------------------------------------//
   Display_on;
   glcd_init(ON);             //Inicializa la glcd
   glcd_fillScreen(OFF);      //Limpia la pantalla
   glcd_text57(0, 0, JP, 2, ON);glcd_text57(5, 20, Cabina, 1, ON);glcd_text57(10, 40, Clase, 2, ON);             
   glcd_update();
   delay_ms(3000);
                       
   glcd_fillScreen(OFF);      //Limpia la pantalla
   Menu=0;
   LuzBlanca_off;Alarma_off;
//-------Inicio de Cabina-------------------------------------------------------------------------//
   VelocidadMotor(0);
   Motor_off;
   LeeEEPROM(); // Lectura de variables almacenadas en la EEPROM 
   LimitaValores();
   //Menu=2;
   while(true){ 
   glcd_update();
//**Actualización de Lectura de Sensores**//   
   if(Lectura==1){
      Lectura=0;
      InFlow=LeerFlujoInFlow(8);
      Temperatura=leeTemperatura(5);
   }
//----------------------------------------//

//**Contador de Tiempo de Trabajo del filtro**//
   if(minutos_trabajo>=3600){ // Tiempo de Trabajo
      minutos_trabajo=0; //Minutos Trabajo realmente son segundos.
      write_eeprom(33,make8(minutos_trabajo,0));
      delay_ms(20);
      write_eeprom(34,make8(minutos_trabajo,1));
      delay_ms(20);
      Ttrabajo[0]++;
      if(Ttrabajo[0]>9){
         Ttrabajo[0]=0;
         Ttrabajo[1]++;
      }
      if(Ttrabajo[1]>9){
         Ttrabajo[1]=0;
         Ttrabajo[2]++;
      }
      if(Ttrabajo[2]>9){
         Ttrabajo[2]=0;
         Ttrabajo[3]++;
      }
      if(Ttrabajo[3]>9){
         Ttrabajo[3]=0;
      }
      write_eeprom(16,Ttrabajo[0]);
      delay_ms(20);
      write_eeprom(17,Ttrabajo[1]);
      delay_ms(20);
      write_eeprom(18,Ttrabajo[2]);
      delay_ms(20);
      write_eeprom(19,Ttrabajo[3]);
      delay_ms(20);
   }else if(minutos_trabajo==900 || minutos_trabajo==1800 || minutos_trabajo==2700 ){
      write_eeprom(33,make8(minutos_trabajo,0));
      delay_ms(20);
      write_eeprom(34,make8(minutos_trabajo,1));
      delay_ms(20);
   }
//----------------------------------------//      
   
   if(Menu!=2){// Si no esta en el menu de estados apaga la alarma
      Alarma_off;
      Alarma=10;
   }
   
//_--------------------------------------------------------------------------------------------------------------------------------------------------------
   if(Menu==0){ //Menu de Contraseña.
      while(PidePassword(0,0,1,Password[0],Password[1],Password[2],Password[3])==0){
         VelocidadMotor(0);
         glcd_update();
      }       
   }
//_--------------------------------------------------------------------------------------------------------------------------------------------------------       

//_--------------------------------------------------------------------------------------------------------------------------------------------------------
      if(Menu==1){//Menu de Purga.
         Purga=ON;
         glcd_text57(15, 1, Tiempo, 1, ON);
         tiempo_purga=1;LuzBlanca_off;//Alarma_off;
         glcd_rect(9, 54, 120, 64, YES, OFF);         
         displayTiempo(Tpurga[3],20,20,0,2);displayTiempo(Tpurga[2],40,20,0,2);glcd_text57(60, 22, Dos_puntos, 2, ON);
         displayTiempo(Tpurga[1],70,20,0,2);displayTiempo(Tpurga[0],90,20,0,2);        
         
         VelocidadMotor(VelMotor);
         
         if(RIGHT){
            delay_ms(500);
            if(RIGHT){
               tiempo_purga=0;Menu=2;glcd_fillScreen(OFF);
            }
         }
       
         if((Tpurga[0]>=Tpurgap[0])&&(Tpurga[1]>=Tpurgap[1])&&(Tpurga[2]>=Tpurgap[2])&&(Tpurga[3]>=Tpurgap[3])){
            tiempo_purga=0;Menu=2;glcd_fillScreen(OFF);flag_luz=!flag_luz;flag_blower=1;flag_toma=1;
         }      
      }
//_--------------------------------------------------------------------------------------------------------------------------------------------------------       

//_--------------------------------------------------------------------------------------------------------------------------------------------------------
      if(Menu==2){ //Menu General.
         Purga=OFF;
         w=0;
         //glcd_imagen(4);
         
         if(VelMotor==1){
            glcd_rect(0, 29, 13, 39, YES, ON);
            glcd_rect(15, 26, 28, 39, NO, ON);
            glcd_rect(30, 23, 43, 39, NO, ON);
            glcd_rect(45, 20, 58, 39, NO, ON);
            glcd_rect(60, 17, 73, 39, NO, ON);
            glcd_rect(75, 14, 88, 39, NO, ON);
         }else if(VelMotor==2){
            glcd_rect(0, 29, 13, 39, YES, ON);
            glcd_rect(15, 26, 28, 39, YES, ON);
            glcd_rect(30, 23, 43, 39, NO, ON);
            glcd_rect(45, 20, 58, 39, NO, ON);
            glcd_rect(60, 17, 73, 39, NO, ON);
            glcd_rect(75, 14, 88, 39, NO, ON);
         }else if(VelMotor==3){
            glcd_rect(0, 29, 13, 39, YES, ON);
            glcd_rect(15, 26, 28, 39, YES, ON);
            glcd_rect(30, 23, 43, 39, YES, ON);
            glcd_rect(45, 20, 58, 39, NO, ON);
            glcd_rect(60, 17, 73, 39, NO, ON);
            glcd_rect(75, 14, 88, 39, NO, ON);
         }else if(VelMotor==4){
            glcd_rect(0, 29, 13, 39, YES, ON);
            glcd_rect(15, 26, 28, 39, YES, ON);
            glcd_rect(30, 23, 43, 39, YES, ON);
            glcd_rect(45, 20, 58, 39, YES, ON);
            glcd_rect(60, 17, 73, 39, NO, ON);
            glcd_rect(75, 14, 88, 39, NO, ON);
         }else if(VelMotor==5){
            glcd_rect(0, 29, 13, 39, YES, ON);
            glcd_rect(15, 26, 28, 39, YES, ON);
            glcd_rect(30, 23, 43, 39, YES, ON);
            glcd_rect(45, 20, 58, 39, YES, ON);
            glcd_rect(60, 17, 73, 39, YES, ON);
            glcd_rect(75, 14, 88, 39, NO, ON);
         }else if(VelMotor==6){
            glcd_rect(0, 29, 13, 39, YES, ON);
            glcd_rect(15, 26, 28, 39, YES, ON);
            glcd_rect(30, 23, 43, 39, YES, ON);
            glcd_rect(45, 20, 58, 39, YES, ON);
            glcd_rect(60, 17, 73, 39, YES, ON);
            glcd_rect(75, 14, 88, 39, YES, ON);
         }
         

         if(imprimir==1){
            glcd_text57(2, 12, Inflowt, 1, ON);
            displayTiempo(VelMotor,25,11,0,1);  
            glcd_rect(0, 10, 95, 63, NO, ON);
            glcd_rect(100, 10, 127, 63, NO, ON);
            glcd_text57(2, 51, Downflowt, 1, ON);
            glcd_text57(2, 41, Vida_Filtro, 1, ON);
            displayTiempo(Ttrabajo[3],60,41,0,1);displayTiempo(Ttrabajo[2],67,41,0,1);displayTiempo(Ttrabajo[1],74,41,0,1);displayTiempo(Ttrabajo[0],82,41,0,1);
            strcpy(Menus.Letra,"JPET120-LC");
            displayMenu(Menus.Letra,20,0,1,1);
            displayfloat(Temperatura,69,50,0,1);
            imprimir=0;
         }
                  
         if(flag_blower==1){
            glcd_imagen(5);tiempo_trabajo=1;
            if(guardatrabajo==1){
               VelocidadMotor(6);
               guardatrabajo=0;
               write_eeprom(33,make8(minutos_trabajo,0));
               delay_ms(20);
               write_eeprom(34,make8(minutos_trabajo,1));
               delay_ms(20);
               write_eeprom(16,Ttrabajo[0]);
               delay_ms(20);
               write_eeprom(17,Ttrabajo[1]);
               delay_ms(20);
               write_eeprom(18,Ttrabajo[2]);
               delay_ms(20);
               write_eeprom(19,Ttrabajo[3]);
               delay_ms(20);
               glcd_init(ON);             //Inicializa la glcd
               glcd_fillScreen(OFF);      //Limpia la pantalla
               glcd_update();  
            }
            VelocidadMotor(VelMotor);
         }else{
            Filtro_Downflow=0;
            glcd_rect(102, 45, 126, 62, YES, OFF);tiempo_trabajo=0;VelocidadMotor(0);
            if(guardatrabajo==0){
               guardatrabajo=1;
               write_eeprom(33,make8(minutos_trabajo,0));
               delay_ms(20);
               write_eeprom(34,make8(minutos_trabajo,1));
               delay_ms(20);
               write_eeprom(16,Ttrabajo[0]);
               delay_ms(20);
               write_eeprom(17,Ttrabajo[1]);
               delay_ms(20);
               write_eeprom(18,Ttrabajo[2]);
               delay_ms(20);
               write_eeprom(19,Ttrabajo[3]);
               delay_ms(20);
               //glcd_init(ON);             //Inicializa la glcd
               //glcd_fillScreen(OFF);      //Limpia la pantalla
               glcd_update();  
            }
         }
         
         if(flag_luz==1){
            glcd_imagen(3);LuzBlanca_on;
         }else{
            glcd_rect(102, 14, 126, 28, YES, OFF);LuzBlanca_off;
         }
         
            
      if(RIGHT){//Si oprime boton de Blower.
         if(Flanco3 == 0){
            Flanco3 = 1;delay_ms(3);flag_blower=!flag_blower;
         }
      }else{
         Flanco3 = 0;
      }
            
      if(DOWN){//Si oprime boton de Luz Blanca.
         if(Flanco5 == 0){
            Flanco5 = 1;delay_ms(3);flag_luz=!flag_luz;
         }
      }else{
         Flanco5 = 0;
      }
      
      if(UP){//Si oprime boton de Toma.
         delay_ms(20);
         if(UP){//Si oprime boton de Toma.
            delay_ms(5);Menu=3;//glcd_fillScreen(OFF);
            glcd_init(ON);             //Inicializa la glcd
            glcd_fillScreen(OFF);      //Limpia la pantalla
            glcd_update();  
            LuzBlanca_off;
         }
      }
            
    }
//_--------------------------------------------------------------------------------------------------------------------------------------------------------       

//_--------------------------------------------------------------------------------------------------------------------------------------------------------
      if(Menu==3){ //Menu Principal.
            
      strcpy(Menus.Letra,"Menu");
      displayMenu(Menus.Letra,40,0,1,2);
      
      if(Opcion==1){
         strcpy(Menus.Letra,"Ajustes");
         displayMenu(Menus.Letra,0,19,1,1);
      }else{
         strcpy(Menus.Letra,"Ajustes");
         displayMenu(Menus.Letra,0,19,0,1);
      }
         
      if(Opcion==2){
         strcpy(Menus.Letra,"Cambio de Clave");
         displayMenu(Menus.Letra,0,35,1,1);
      }else{
         strcpy(Menus.Letra,"Cambio de Clave");
         displayMenu(Menus.Letra,0,35,0,1);
      }
         
      if(Opcion==3){
         strcpy(Menus.Letra,"Modo");
         displayMenu(Menus.Letra,0,51,1,1);
      }else{
         strcpy(Menus.Letra,"Modo");
         displayMenu(Menus.Letra,0,51,0,1);
      }
      glcd_update(); 
      
      if(w==0){
         delay_ms(500);w=1;
      }
         
      if(UP){//Si oprime hacia arriba        
         if(Flanco == 0){
            Flanco = 1;delay_ms(30);Opcion--;
         }
      }else{
         Flanco = 0;
      }
            
      if(DOWN){//Si oprime hacia abajo
         if(Flanco2 == 0){
            Flanco2 = 1;delay_ms(30);Opcion++;
         }
      }else{
         Flanco2 = 0;
      }
            
      if(RIGHT){//Si oprime SET
         if(Flanco1 == 0){
            Flanco1 = 1;delay_ms(500);Menu=Opcion+3;Opcion=1;glcd_fillscreen(OFF);unidad=1;
            Contrasena[0]=0;Contrasena[1]=0;Contrasena[2]=0;Contrasena[3]=0;n=0;
         }
      }else{
         Flanco1 = 0;
      }
            
      if(LEFT){//Si oprime boton de Toma.
         delay_ms(700);Menu=2;glcd_fillscreen(OFF);
      }
            
      if(Opcion>3)
         Opcion=1;
      if(Opcion<1)
         Opcion=3;
    }
//_--------------------------------------------------------------------------------------------------------------------------------------------------------       

//_--------------------------------------------------------------------------------------------------------------------------------------------------------
      if(Menu==4){ //Menu de Ajustes.
      
      strcpy(Menus.Letra,"Ajustes");
      displayMenu(Menus.Letra,0,0,1,2);
      
      if(Opcion==1)   
         {strcpy(Menus.Letra,"Tiempo Purga");
         displayMenu(Menus.Letra,0,20,1,1);}
      else
         {strcpy(Menus.Letra,"Tiempo Purga");
         displayMenu(Menus.Letra,0,20,0,1);}             
        
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
      
      if(Opcion>1)
      {Opcion=1;}
      if(Opcion<1)
      {Opcion=1;}
    
    }
//_--------------------------------------------------------------------------------------------------------------------------------------------------------       

//_--------------------------------------------------------------------------------------------------------------------------------------------------------
      if(Menu==6){ //Menu de Modo.
      
      strcpy(Menus.Letra,"Modo");
      displayMenu(Menus.Letra,40,0,1,2);
         
      if(Opcion==1)   
         {strcpy(Menus.Letra,"Mantenimiento");
         displayMenu(Menus.Letra,0,19,1,1);}
      else
         {strcpy(Menus.Letra,"Mantenimiento");
         displayMenu(Menus.Letra,0,19,0,1);}   
         
      if(Opcion==2)   
         {strcpy(Menus.Letra,"Servicio");
         displayMenu(Menus.Letra,0,35,1,1);}
      else
         {strcpy(Menus.Letra,"Servicio");
         displayMenu(Menus.Letra,0,35,0,1);}
         
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
            {Flanco1 = 1;delay_ms(500);Menu=Opcion+30;glcd_fillScreen(OFF);Opcion=1;unidad=1;
            }
      }
         else
            {Flanco1 = 0;}  
            
      if(LEFT)//Si oprime boton de Toma.
        {delay_ms(700);Menu=3;glcd_fillscreen(OFF);}
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
      {
         delay_ms(700);Menu=4;glcd_fillscreen(OFF);Opcion=1;unidad=1;
         write_eeprom(4,Tpurgap[0]);
         delay_ms(20);
         write_eeprom(5,Tpurgap[1]);
         delay_ms(20);
         write_eeprom(6,Tpurgap[2]);
         delay_ms(20);
         write_eeprom(7,Tpurgap[3]);
         delay_ms(20);
      }
      
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
      if(Menu==5){ //Menu de Clave.

      PidePassword(3,5,211,Password[0],Password[1],Password[2],Password[3]);
            
      if(LEFT)//Si oprime boton de Toma.
      {
         delay_ms(200);
         if(unidad==1){
            Menu=3;glcd_fillscreen(OFF);Opcion=1;unidad=1;Contrasena[0]=0;Contrasena[1]=0;Contrasena[2]=0;Contrasena[3]=0;
         }
      } 
    }
//_--------------------------------------------------------------------------------------------------------------------------------------------------------       

//_--------------------------------------------------------------------------------------------------------------------------------------------------------
      if(Menu==211){ //Menu de Cambio Clave.
      
      if(n==0)
      {
         displayContrasena(Contrasena[0],20,30,1);displayContrasena(Contrasena[1],40,30,0);
         displayContrasena(Contrasena[2],60,30,0);displayContrasena(Contrasena[0],80,30,0);
         n=1;
      }
      strcpy(Menus.Letra,"Clave Nueva");
      displayMenu(Menus.Letra,0,0,1,2);
        
      
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
            
      if(DOWN )//Si oprime hacia abajo
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
         if(unidad==1){
            Menu=3;glcd_fillscreen(OFF);Opcion=1;unidad=1;Contrasena[0]=0;Contrasena[1]=0;Contrasena[2]=0;Contrasena[3]=0;
         }
      }
      
      if(unidad>4)
      {
         Opcion=1;glcd_fillscreen(OFF);unidad=1;n=0;
         Password[0]=Contrasena[0];Password[1]=Contrasena[1];Password[2]=Contrasena[2];Password[3]=Contrasena[3];
         write_eeprom(0,Password[0]);
         delay_ms(20);
         write_eeprom(1,Password[1]);
         delay_ms(20);
         write_eeprom(2,Password[2]);
         delay_ms(20);
         write_eeprom(3,Password[3]);
         delay_ms(20);
         glcd_imagen(1);glcd_update();Menu=211;delay_ms(1000);glcd_fillScreen(OFF);Menu=3;
         n=0;
         Contrasena[0]=0;Contrasena[1]=0;Contrasena[2]=0;Contrasena[3]=0;
      }
    
    }
//_--------------------------------------------------------------------------------------------------------------------------------------------------------       

//_--------------------------------------------------------------------------------------------------------------------------------------------------------
      if(Menu==31){ //Menu de Mantenimiento.
      
         strcpy(Menus.Letra,"Manteniento");
         displayMenu(Menus.Letra,0,0,1,2);
         
         strcpy(Menus.Letra,"Trabajo Filtro(h):");
         displayMenu(Menus.Letra,10,20,1,1);
         glcd_text57(0, 31, Vida_Filtro, 1, ON);
         displayTiempo(Ttrabajo[3],60,30,0,1);displayTiempo(Ttrabajo[2],67,30,0,1);displayTiempo(Ttrabajo[1],74,30,0,1);displayTiempo(Ttrabajo[0],82,30,0,1);
         
         LuzBlanca_on;
         
            if(UP && RIGHT) // Reinicia Tiempo de Luz UV
            {
                  delay_ms(2000);
                  write_eeprom(12,0);
                  delay_ms(20);
                  write_eeprom(13,0);
                  delay_ms(20);
                  write_eeprom(14,0);
                  delay_ms(20);
                  write_eeprom(15,0);
                  delay_ms(20);
                  Tuv[3]=0;Tuv[2]=0;Tuv[1]=0;Tuv[0]=0;
                  Menu=3;glcd_fillscreen(OFF);LuzBlanca_off;
               
            }
            if(DOWN && RIGHT) // Reinicia Tiempo de Trabajo
            {
                  delay_ms(2000);
                  write_eeprom(16,0);
                  delay_ms(20);
                  write_eeprom(17,0);
                  delay_ms(20);
                  write_eeprom(18,0);
                  delay_ms(20);
                  write_eeprom(19,0);
                  delay_ms(20);
                  Ttrabajo[3]=0;Ttrabajo[2]=0;Ttrabajo[1]=0;Ttrabajo[0]=0;
                  Menu=3;glcd_fillscreen(OFF);LuzBlanca_off;
            }
            
            if(LEFT)
            {
               Menu=6;glcd_fillscreen(OFF);LuzBlanca_off;Opcion=1;unidad=1;
            }
    }
//_--------------------------------------------------------------------------------------------------------------------------------------------------------       

//_--------------------------------------------------------------------------------------------------------------------------------------------------------
      if(Menu==32){ //Menu de Servicio
      
      strcpy(Menus.Letra,"Servicio");
      displayMenu(Menus.Letra,10,0,1,2);
      
      if(Opcion==1)   
         {strcpy(Menus.Letra,"Sensor de Flujo");
         displayMenu(Menus.Letra,0,18,1,1);}
      else
         {strcpy(Menus.Letra,"Sensor de Flujo");
         displayMenu(Menus.Letra,0,18,0,1);} 
         
      if(Opcion==2)   
         {strcpy(Menus.Letra,"Sensor de Presion");
         displayMenu(Menus.Letra,0,29,1,1);}
      else
         {strcpy(Menus.Letra,"Sensor de Presion");
         displayMenu(Menus.Letra,0,29,0,1);}  
      
      if(Opcion==3)   
         {strcpy(Menus.Letra,"Velocidad Ventilador");
         displayMenu(Menus.Letra,0,40,1,1);}
      else
         {strcpy(Menus.Letra,"Velocidad Ventilador");
         displayMenu(Menus.Letra,0,40,0,1);} 
      
        
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
      {  
         delay_ms(20);
         if(RIGHT){
            delay_ms(100);
            glcd_fillScreen(OFF);
            while(!PidePassword(6,32,Opcion+40,4,4,4,4)){
               PidePassword(6,32,Opcion+40,4,4,4,4);glcd_update();
            }            
            glcd_fillScreen(OFF);Opcion=1;unidad=1;
         }
      }
            
      if(LEFT)//Si oprime boton de Toma.
        {delay_ms(700);Menu=6;glcd_fillscreen(OFF);Opcion=1;unidad=1;}
        
      if(Opcion>3)
      {Opcion=1;}
      if(Opcion<1)
      {Opcion=3;}
    }
//_--------------------------------------------------------------------------------------------------------------------------------------------------------       

//_--------------------------------------------------------------------------------------------------------------------------------------------------------
      if(Menu==41){ //Menu de Sensor de Flujo.
      
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
//      displayfloat(Diferencia,0,50,0,1);
      
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
            negativo=10;write_eeprom(50,negativo);delay_ms(20);
         }
         if(Diferencia<0)
         {
            negativo=20;write_eeprom(50,negativo);delay_ms(20);
         }
         Diferencia=abs(Diferencia);
         Temporal1=(int16)Diferencia;
         write_eeprom(60,make8(Temporal1,0));
         delay_ms(20);
         write_eeprom(61,make8(Temporal1,1));//Guardar valor de Setpoint en eeprom
         delay_ms(20);

         Temporal1=(int16)zero_actual;
         write_eeprom(29,make8(Temporal1,0));
         delay_ms(20);
         write_eeprom(30,make8(Temporal1,1));//Guardar valor de Setpoint en eeprom
         delay_ms(20);
      }
         else
            {Flanco1 = 0;}  
            
      if(LEFT)//Si oprime boton de Toma.
      {
         delay_ms(700);Menu=32;glcd_fillscreen(OFF);Opcion=1;unidad=1;
         Temporal=Ajuste1;
         Entero=(int)Ajuste1;
         Temporal=Temporal-Entero;
         Temporal2=Temporal*10.0;
         Decimal1=(int8)Temporal2;
         write_eeprom(25,Entero);
         delay_ms(20);
         write_eeprom(26,Decimal1);
         delay_ms(20);
      }
    }
//_--------------------------------------------------------------------------------------------------------------------------------------------------------       

//_--------------------------------------------------------------------------------------------------------------------------------------------------------
      if(Menu==42){ //Menu de Sensor de Presión.
      
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
            
         write_eeprom(62,G_l);
         delay_ms(20);
         write_eeprom(63,G_h);//Guardar valor de Setpoint en eeprom
         delay_ms(20);
      }
         else
            {Flanco1 = 0;}  
            
      if(LEFT)//Si oprime boton de Toma.
      {                  
         delay_ms(700);Menu=32;glcd_fillscreen(OFF);Opcion=1;unidad=1;
         Temporal=Ajuste2;
         Entero=(int)Ajuste2;
         Temporal=Temporal-Entero;
         Temporal2=Temporal*10.0;
         Decimal1=(int8)Temporal2;
         write_eeprom(27,Entero);
         delay_ms(20);
         write_eeprom(28,Decimal1);
         delay_ms(20);
      }
    }
//_--------------------------------------------------------------------------------------------------------------------------------------------------------       

//_--------------------------------------------------------------------------------------------------------------------------------------------------------
      if(Menu==43){ //Menu de Velocidad de Motor
      
      if(VelMotor>6)VelMotor=6;
      
      strcpy(Menus.Letra,"Seleccion Vel");
      displayMenu(Menus.Letra,10,0,1,1);
      
      displayTiempo(VelMotor,50,40,1,2);
      
      if(UP && VelMotor<6)//Si oprime hacia arriba
      {  
         delay_ms(200);VelMotor++;
      }
            
      if(DOWN && VelMotor>0)//Si oprime hacia abajo
      {  
         delay_ms(200);VelMotor--;
      }
            
      if(RIGHT)//Si oprime SET
      {           
         VelocidadMotor(VelMotor);
      } 
            
      if(LEFT)//Si oprime boton de Toma.
      {                  
         delay_ms(100);glcd_fillscreen(OFF);Opcion=1;unidad=1;
         write_eeprom(32,VelMotor);//Guardar valor de Setpoint en eeprom
         delay_ms(20);
         glcd_fillScreen(OFF);
         strcpy(Menus.Letra,"Velocidad");
         displayMenu(Menus.Letra,0,0,0,2);
         strcpy(Menus.Letra,"Guardada");
         displayMenu(Menus.Letra,0,30,0,2);
         glcd_update();
         delay_ms(1000);
         glcd_fillScreen(OFF);
         Menu=32;
      }
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

