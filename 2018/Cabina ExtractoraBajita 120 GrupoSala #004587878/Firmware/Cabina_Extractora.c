////////////////////////////////////////////////////////////////////////////
////        (C) Copyright 2018 JP Bioingenieria SAS                     ////
////              JUDAPIES-dielecingenieria.com                         ////
////////////////////////////////////////////////////////////////////////////

#include <18F4550.h>
#device adc=10
//#define SIMU 1
#ifdef SIMU
#fuses XT,NOWDT,NOPROTECT,NOLVP,NODEBUG,NOMCLR// PLL1 para 4 MHz
#use delay(clock=4000000)
#else
#fuses XTPLL,NOWDT,NOPROTECT,NOLVP,NODEBUG,USBDIV,PLL1,CPUDIV1,VREGEN,NOMCLR// PLL1 para 4 MHz
#use delay(clock=48000000)
#define FAST 1
#endif
#define use_portd_lcd TRUE
#include <LCD420PIC18F.c>
//#include <LCD420.c>
#include <math.h>
//#define DAMPER 1

// Definición de teclado - NO
#define   RIGHT         input(PIN_E1)
#define   LEFT          input(PIN_E2)
#define   DOWN          input(PIN_E0)
#define   UP            input(PIN_A5)

// Definición de otros puertos

#define   Luz_Blanca_on     output_bit(PIN_B4,1)
#define   Luz_Blanca_off    output_bit(PIN_B4,0)
#define   Motor_on          output_bit(PIN_B5,1)
#define   Motor_off         output_bit(PIN_B5,0)
#define   Alarma_on         output_bit(PIN_B6,1)
#define   Alarma_off        output_bit(PIN_B6,0)
#define   Damper_on         output_bit(PIN_B7,1)
#define   Damper_off        output_bit(PIN_B7,0)
#define   Pantalla_on       output_bit(PIN_C1,1)
#define   Pantalla_off      output_bit(PIN_C1,0)

#ifdef DAMPER
int8 n_opcionH=11;
#else
int8 n_opcionH=7;
#endif
int8 Menu=100,n_opcionL=2,unidad=11,Flecha=2,Flecha1=3,Flecha2=2, nMenuL=2, nMenuH=4,contraste=10,unidad2=1,purga_minutos=0,purga_minutos_pro=0;
int8 Entero=0,Decimal1=0,Decimal2=0,Entero1=0,l=0,h=0,tmp=10,MediaMovil=0,negativo=10,Automa=10,Manual=10,Semi=10,ValorSemi=0;
int8 segundos=0,minutos=0,horas=0,tiemporeset=0,purga_segundos=0,purga_segundos_pro=0,ppurga_segundos=0,ppurga_segundos_pro=0,ppurga_minutos=0,ppurga_minutos_pro=0;
int8 FeedbackDamper=0;
short estadofl=0,estadomv=0,estadoalarma=0,estadopantalla=0,Flanco=0,Flanco1=0,Flanco2=0,Flanco3=0,flag_latencia=0,Latencia=0,flag_filtro=0;
short imprimio=0,Ruido=0;
signed int8   paso=0;
char t[3]={'>',' ','^'}; 
signed  int8 clave[4]={0,0,0,0};   // 0=Verdadero, 1=Falso
int8 contrasena[4]={0,0,0,0};   // 0=Verdadero, 1=Falso
float setpoint=0.0,zero_fabrica=203.0;
short ModoSleep,Lectura=0,Lectura2=0,flag_iniciodamper=0,iniciodamper=0,ModoManual=0,ModoAuto=0,ModoSemi=0,tiempo_purga=0,tiempo_ppurga=0;
int16 tiempos=0,t_latencia=0,tiempos2=0,Dif16=0;
//float a=24.9616,b=-175.3954,c=489.5268,d=-440.7857;
char letra; 
//float a=-0.00000007109886647647915,b=0.0003576135181122329,c= -0.169922240146291,d= 19.248973132140719;//Sensor 1 Posible dañado
float a=0.0000001418633343546420,b=-0.00009476749001431169,c= 0.040182822903506,d= -5.354852229527197;//Sensor 2 Posible bueno
float Flujo=0.0,Flujo2=0.0,x=0.0,x_uno=0.0,x_cuadrado=0.0,x_cubo=0.0,Temporal=0.0,Temporal2=0.0,Velocidad=0.0;
float Temporal1=0.0,Diferencia=0.0,Ajuste1=1.0;
float error=0.0;
float promedio[12]={0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};

#include <Funciones.h>

#int_TIMER1
void temp1s(void){
   #ifdef SIMU
   set_timer1(60536);  // 5  ms
   #else
   set_timer1(5536);  // 5  ms
   #endif
   tiempos++;
   tiempos2++;
   
   if(t_latencia>=3000)
      Latencia=1;
   
   if(flag_latencia==1)
      t_latencia++;         
   
   if(tiempos2==200){
      tiempos2=0;Lectura2=1;
   }
   
   if(tiempos==200){   // Ha transcurrido una decima de segundo (PIC18F4550 con XT = 16MHZ)
      tiempos=0;tiemporeset++;
      Lectura=1;
         
      if(estadomv==1 && flag_filtro==1){  
         segundos++;
         if(segundos==60){
            segundos=0;minutos++;
         } 
         if(minutos==60){
            minutos=0;horas++;
         }
      }
      
      if(tiempo_purga==1){
         if(purga_segundos>0){
            purga_segundos--;
         }else{
            if(purga_minutos>0){
               purga_minutos--;
               purga_segundos=59;
            }else{
               //tiempo_cumplido=1;
               tiempo_purga=0;
               //Motor_on;
               //EnciendeMotor();
               Menu=20;
               Luz_Blanca_on;
               estadomv=1;
               estadofl=1;
            }
         }
      }
      
      if(tiempo_ppurga==1){
         if(ppurga_segundos>0){
            ppurga_segundos--;
         }else{
            if(ppurga_minutos>0){
               ppurga_minutos--;
               ppurga_segundos=59;
            }else{
               //tiempo_cumplido=1;
               reset_cpu();
            }
         }
      }
   }
}

void main ()
{  
   ConfigInicial();
   LeeEEPROM();            
   LimitaValores(); 
   Menu=200;
   SalidasOff();
   while(true){

   while(Menu==200){
      SalidasOff();
      if(RIGHT){
         delay_ms(1000);
         if(RIGHT){   
            lcd_init();
            intensidadpantalla(1);
            lcd_gotoxy(1,1);
            printf(lcd_putc," CABINA  EXTRACTORA ");
            lcd_gotoxy(1,2);
            printf(lcd_putc,"  DE HUMOS Y GASES  ");
            lcd_gotoxy(1,3);
            printf(lcd_putc,"    JP INGLOBAL     ");
            lcd_gotoxy(1,4);
            printf(lcd_putc," REF: PANEL JPCEH15 ");
            delay_ms(2000);
            printf(lcd_putc,"\f");
            Menu=0;
            delay_ms(500);
         }
      }
   }

//------------Menu0------------------------------------------------------------------   
   while(Menu == 0){ // Menu de Contraseña para Poder iniciar el equipo
      flag_latencia=1;
      SalidasOff();
      if(Latencia==1){
         printf(lcd_putc,"\f");reset_cpu();
      }
      lcd_gotoxy(1,1);
      printf(lcd_putc,"     Ingrese        ");
      lcd_gotoxy(1,2);
      printf(lcd_putc,"    Contraseña      ");
      lcd_gotoxy(1,3);
      printf(lcd_putc,"    CLAVE=%i%i%i%i  ",clave[0],clave[1],clave[2],clave[3]);
  
      //letra=lcd_getc(5,3);
      
      if(UP){//Si oprime hacia arriba
         delay_ms(20);
         if(UP && Flanco == 0){
            clave[unidad-11]++;Flanco = 1;delay_ms(30);t_latencia=0;
         }
      }else{
         Flanco = 0;
      }
            
      if(DOWN){//Si oprime hacia abajo
         delay_ms(20);
         if(DOWN && Flanco2 == 0){
            clave[unidad-11]--;Flanco2 = 1;delay_ms(30);t_latencia=0;
         }
      }else{
         Flanco2 = 0;
      }
   
      if(RIGHT){// Si Oprime Derecha
         delay_ms(20);
         if(RIGHT && Flanco1 == 0){
            Flanco1 = 1;unidad++;delay_ms(30);printf(lcd_putc,"\f");t_latencia=0;
         }
      }else{
         Flanco1 = 0;
      }
            
      if(LEFT){// Si Oprime izquierda
         delay_ms(20);
         if(LEFT && Flanco3 == 0){
            Flanco3 = 1;unidad--;delay_ms(30);printf(lcd_putc,"\f");t_latencia=0;
         }
      }else{
         Flanco3 = 0;
      }
            
      if(clave[unidad-11]<0)     // Si la unidad donde se encuentra ubicado el cursor es menor que 0 pasa a 9.
         clave[unidad-11]=9;
      if(clave[unidad-11]>9)     // Si la unidad donde se encuentra ubicado el cursor es mayor que 9 pasa a 0.
         clave[unidad-11]=0;
      if(unidad<11)             // Si trata de correr mas a la izquierda de la primera unidad, deja el cursor en esa posicion.
         unidad=11;
      
      lcd_gotoxy(unidad,4);// Para mostrar cursor.
      lcd_putc(t[2]);
      
      if(unidad>14){             // Si a Terminado de ingresar la clave, verifica si es correcta o no.
         if(clave[0]==3&&clave[1]==8&&clave[2]==9&&clave[3]==2){ // Si Ingresa clave para reset general del sistema.
             write_eeprom(0,0);delay_ms(20);write_eeprom(1,0);delay_ms(20);// Reestablece a contraseña de Fabrica y reinicia Programa.
             write_eeprom(2,0);delay_ms(20);write_eeprom(3,0);delay_ms(20);
             reset_cpu();
         }
            
         if(clave[0]==contrasena[0]&&clave[1]==contrasena[1]&&clave[2]==contrasena[2]&&clave[3]==contrasena[3]){ // Si las claves coinciden pasa a Menu Principal.
            lcd_gotoxy(1,1);
            printf(lcd_putc,"                   ");
            lcd_gotoxy(1,2);
            printf(lcd_putc,"     Contraseña    ");
            lcd_gotoxy(1,3);
            printf(lcd_putc,"      Correcta     ");
            lcd_gotoxy(1,4);
            printf(lcd_putc,"                   ");
            delay_ms(500);Menu=50;unidad=11;printf(lcd_putc,"\f");Damper_on;flag_latencia=0;Latencia=0;tiempo_purga=1;
            t_latencia=0;flag_iniciodamper=1;
            clave[0]=0;clave[1]=0;clave[2]=0;clave[3]=0;
         }else{                                         // Si la clave no coincide vuelve a mostrar el menu para ingresar la clave.
            lcd_gotoxy(1,1);
            printf(lcd_putc,"");
            lcd_gotoxy(1,2);
            printf(lcd_putc,"     Contraseña    ");
            lcd_gotoxy(1,3);
            printf(lcd_putc,"     Incorrecta    ");
            lcd_gotoxy(1,4);
            printf(lcd_putc,"                   ");
            delay_ms(500);unidad=11;printf(lcd_putc,"\f");
            clave[0]=0;clave[1]=0;clave[2]=0;clave[3]=0;
         }
      }
   
      if(unidad>11&&unidad<14){
         lcd_gotoxy(unidad-1,4);// Para mostrar cursor.
         lcd_putc(t[1]);
      }

   }
//----------------Fin-Menu0---------------------------------------------------------------   

//------------Menu1------------------------------------------------------------------   
   if(Menu == 1){ // Menu de seleccion de lo que desea encender
   
      lcd_gotoxy(1,1);
      printf(lcd_putc,"---MENU PRINCIPAL---");
   
      if(UP){//Si oprime hacia arriba
         delay_ms(20);
         if(UP && Flanco == 0){
            Flecha2--;Flecha--;Flecha1=Flecha+1;Flanco = 1;delay_ms(30);
         }
      }else{
         Flanco = 0;
      }
            
      if(DOWN){//Si oprime hacia abajo
         delay_ms(20);
         if(DOWN && Flanco2 == 0){
            Flecha2++;Flecha++;Flecha1=Flecha-1;Flanco2 = 1;delay_ms(30);
         }
      }else{
         Flanco2 = 0;
      }
   
        
      if(Flecha2>nMenuH)
      {paso++;nMenuH=Flecha2;nMenuL=nMenuH-2;Flecha=4;}
        
      if(Flecha2<nMenuL)
      {paso--;nMenuL=Flecha2;nMenuH=nMenuL+2;Flecha=2;}
        
      if(Flecha2>n_opcionH)
      {Flecha2=n_opcionL;Flecha=2;paso=0;nMenuL=Flecha2;nMenuH=nMenuL+2;}
      
      if(Flecha2<n_opcionL)
      {Flecha2=n_opcionH;Flecha=4;paso=n_opcionH-4;nMenuH=Flecha2;nMenuL=nMenuH-2;}  
         
      if(paso<0)
         paso=0;
      
      mensajes(1+paso,2);
      mensajes(2+paso,3);
      mensajes(3+paso,4);

      lcd_gotoxy(1,Flecha);// Para mostrar la flecha de seleccion
      lcd_putc(t[0]);

      if(Flecha==2){
         lcd_gotoxy(1,4);// Para mostrar la flecha de seleccion
         lcd_putc(t[1]);
         lcd_gotoxy(1,3);// Para mostrar la flecha de seleccion
         lcd_putc(t[1]);
      }
        
      if(Flecha==4){
         lcd_gotoxy(1,2);// Para mostrar la flecha de seleccion
         lcd_putc(t[1]);
         lcd_gotoxy(1,3);// Para mostrar la flecha de seleccion
         lcd_putc(t[1]);
      }
        
      if(Flecha==3){
         lcd_gotoxy(1,4);// Para mostrar la flecha de seleccion
         lcd_putc(t[1]);
         lcd_gotoxy(1,2);// Para mostrar la flecha de seleccion
         lcd_putc(t[1]);
      }

      if(RIGHT){// Si oprime derecha
         delay_ms(20);
         if(RIGHT && Flanco1 == 0) {
            /*Flanco1 = 1;*/Menu=Flecha2;Flecha=3;delay_ms(500);printf(lcd_putc,"\f");
            if(Menu==6){
               clave[0]=0;clave[1]=0;clave[2]=0;clave[3]=0;
            }
         }
      }else{
         Flanco1 = 0;
      }
            
      if(LEFT){// Si oprime Izquierda
         delay_ms(20);
         if(LEFT && Flanco3 == 0) {
            /*Flanco3 = 1;*/delay_ms(500);Menu=20;printf(lcd_putc,"\f");
         }
      }else{
         Flanco3 = 0;
      }
   }
//----------------Fin-Menu1---------------------------------------------------------------      

//----------------Menu2---------------------------------------------------------------
   if(Menu == 2){ // Menu de seleccion de estado de Alarma
   estadoalarma=!estadoalarma;Flanco1 = 1;Menu=1; paso=0;
                      
            if(estadoalarma==1){
               estadoalarma=1;
               lcd_gotoxy(1,1);
               printf(lcd_putc,"                    ");
               lcd_gotoxy(1,2);
               printf(lcd_putc,"        Activo      ");
               lcd_gotoxy(1,3);
               printf(lcd_putc,"        Alarma      ");
               lcd_gotoxy(1,4);
               printf(lcd_putc,"                    ");
            }
               
            if(estadoalarma==0){
               estadoalarma=0;
               lcd_gotoxy(1,1);
               printf(lcd_putc,"                    ");
               lcd_gotoxy(1,2);
               printf(lcd_putc,"      Desactivo     ");
               lcd_gotoxy(1,3);
               printf(lcd_putc,"       Alarma       ");
               lcd_gotoxy(1,4);
               printf(lcd_putc,"                    ");
            }
            write_eeprom(5,estadoalarma);delay_ms(1000);Flecha=2;Flecha2=2;printf(lcd_putc,"\f");
   }      
//----------------Fin-Menu2---------------------------------------------------------------

//----------------Menu3---------------------------------------------------------------
   if(Menu == 3){ // Menu de seleccion de SetPoint
   
      lcd_gotoxy(1,1);
      printf(lcd_putc,"  Duracion Actual   ");                          
      lcd_gotoxy(1,2);                                                  
      printf(lcd_putc," Tiempo= %02u:%02u:%02u ",horas,minutos,segundos);      
      
      if(flag_filtro==1){
         lcd_gotoxy(1,3);                                                 
         printf(lcd_putc,"DESACTIVAR= Oprima >");                        
      }else{
         lcd_gotoxy(1,3);                                                 
         printf(lcd_putc," ACTIVAR= Oprima >  ");                        
      }
   
      lcd_gotoxy(1,4);                                                 
      printf(lcd_putc," RESET= Oprima ^    ");                                                                                   
   
      if(UP){//Si oprime hacia arriba
         delay_ms(20);
         if(UP){
            printf(lcd_putc,"\f");
            lcd_gotoxy(1,2);
            printf(lcd_putc," Reset de tiempo ");
            lcd_gotoxy(1,3);
            printf(lcd_putc,"     Exitoso     ");
            write_eeprom(70,0);delay_ms(20);
            write_eeprom(71,0);delay_ms(20);
            write_eeprom(72,0);delay_ms(20);
            delay_ms(700);
            segundos=0;minutos=0;horas=0;
            delay_ms(30);Menu=1; paso=0;Flecha=2;Flecha2=2;printf(lcd_putc,"\f");
         }
      }
      
      if(RIGHT){
         delay_ms(20);
         if(RIGHT){
            flag_filtro=!flag_filtro;write_eeprom(73,flag_filtro);delay_ms(20);
            delay_ms(500);Menu=1; paso=0;Flecha=2;Flecha2=2;printf(lcd_putc,"\f");
         }
      }
            
      if(LEFT){// Si oprime Izquierda
         delay_ms(20);
         if(LEFT && Flanco3 == 0) {
            Flanco3 = 1;delay_ms(500);Menu=1; paso=0;Flecha=2;Flecha2=2;printf(lcd_putc,"\f");
         }
      }else{
         Flanco3 = 0;
      }
   }      
//----------------Fin-Menu3---------------------------------------------------------------

#ifdef DAMPER
//----------------Menu4---------------------------------------------------------------
   if(Menu == 4){ // Menu de seleccion de SetPoint
      lcd_gotoxy(1,2);
      printf(lcd_putc," Setpoint de  Flujo ");
      lcd_gotoxy(1,3);
      printf(lcd_putc,"       %2.2f   ",setpoint);
   
      if(setpoint>2.0)
         setpoint=2.0;
   
      if(setpoint<0.5)
         setpoint=0.5;
   
      if(UP){//Si oprime hacia arriba
         delay_ms(20);
         if(UP && Flanco == 0) {
            setpoint+=0.1;Flanco = 1;
         }
      }else{
         Flanco = 0;
      }
            
      if(DOWN){//Si oprime hacia abajo
         delay_ms(20);
         if(DOWN && Flanco2 == 0) {
            setpoint-=0.1;Flanco2 = 1;
         }
      }else{
         Flanco2 = 0;
      }
   
      if(RIGHT){// Si oprime derecha
         delay_ms(20);
         if(RIGHT && Flanco1 == 0){
            /*Flanco1 = 1;*/Menu=1;paso=0;Flecha=2;Flecha2=2;unidad=11;delay_ms(500);printf(lcd_putc,"\f");
            Temporal=setpoint;
            Entero=(int)setpoint;
            Temporal=Temporal-Entero;
            Temporal2=Temporal*10.0;
            Decimal2=(int8)Temporal2;
            write_eeprom(40,Entero);delay_ms(20);
            write_eeprom(41,Decimal2);delay_ms(20);//Guardar valor de Setpoint en eeprom
            MensajeGuardado();
         }
      }else{
         Flanco1 = 0;
      }
            
      if(LEFT){// Si oprime Izquierda
         delay_ms(20);
         if(LEFT && Flanco3 == 0) {
            /*Flanco3 = 1;*/Menu=1;paso=0;Flecha=2;Flecha2=2;unidad=11;delay_ms(500);printf(lcd_putc,"\f");
         }
      }else{
         Flanco3 = 0;
      }             
   }      
//----------------Fin-Menu4---------------------------------------------------------------
#else
//----------------Menu4---------------------------------------------------------------
   if(Menu == 4){ // Menu de seleccion de SetPoint
   lcd_gotoxy(1,2);
   printf(lcd_putc,"    Vida Filtro     ");
   lcd_gotoxy(1,3);
   printf(lcd_putc,"      %03.0f Horas  ",setpoint);
   
   if(setpoint<1.0)
   {setpoint=1.0;}
   
   if(setpoint>255.0)
   {setpoint=255.0;}
   
      if(UP)//Si oprime hacia arriba
      {setpoint+=1.0;Flanco = 1;delay_ms(30);}
     
            
      if(DOWN)//Si oprime hacia abajo
      {setpoint-=1.0;Flanco2 = 1;delay_ms(30);}
        
   
      if(RIGHT)// Si oprime derecha
      {  if(Flanco1 == 0) 
            {/*Flanco1 = 1;*/Menu=1;paso=0;Flecha=2;Flecha2=2;unidad=11;delay_ms(500);printf(lcd_putc,"\f");
            Temporal=setpoint;
            Entero=(int)setpoint;
            write_eeprom(40,Entero);
            }}
         else
            {Flanco1 = 0;}   
            
       if(LEFT)// Si oprime Izquierda
      {  if(Flanco3 == 0) 
            {/*Flanco3 = 1;*/Menu=1;paso=0;Flecha=2;Flecha2=2;unidad=11;delay_ms(500);printf(lcd_putc,"\f");}}
         else
            {Flanco3 = 0;}    
             
   }      
//----------------Fin-Menu4---------------------------------------------------------------
#endif
//----------------Menu5---------------------------------------------------------------
   if(Menu == 5){ // Menu para Cambio de Contraseña
      lcd_gotoxy(1,1);
      printf(lcd_putc,"      Ingrese       ");
      lcd_gotoxy(1,2);
      printf(lcd_putc," Contraseña Actual  ");
      lcd_gotoxy(1,3);
      printf(lcd_putc,"    CLAVE=%i%i%i%i  ",clave[0],clave[1],clave[2],clave[3]);
   
      
      if(UP){//Si oprime hacia arriba
         delay_ms(20);
         if(UP && Flanco == 0) {
            clave[unidad-11]++;Flanco = 1;delay_ms(30);
         }
      }else{
         Flanco = 0;
      }
            
      if(DOWN){//Si oprime hacia abajo
         delay_ms(20);
         if(DOWN && Flanco2 == 0) {
            clave[unidad-11]--;Flanco2 = 1;delay_ms(30);
         }
      }else{
         Flanco2 = 0;
      }
   
      if(RIGHT){// Si oprime derecha
         delay_ms(20);
         if(RIGHT && Flanco1 == 0) {
            Flanco1 = 1;unidad++;delay_ms(30);printf(lcd_putc,"\f");
         }
      }else{
         Flanco1 = 0;
      }
            
      if(LEFT){// Si oprime Izquierda
         delay_ms(20);
         if(LEFT && Flanco3 == 0) {
            Flanco3 = 1;unidad--;delay_ms(500);printf(lcd_putc,"\f");
         }
      }else{
         Flanco3 = 0;
      }
            
      if(clave[unidad-11]<0)// Si la unidad donde se encuentra ubicado el cursor es menor que 0 pasa a 9.     
         clave[unidad-11]=9;
      if(clave[unidad-11]>9)// Si la unidad donde se encuentra ubicado el cursor es mayor que 9 pasa a 0.     
         clave[unidad-11]=0;
      if(unidad<11){
         delay_ms(500);Menu=1;paso=0;Flecha=2;Flecha2=2;
         clave[0]=0;clave[1]=0;clave[2]=0;clave[3]=0;
         printf(lcd_putc,"\f");
      }
      lcd_gotoxy(unidad,4);// Para mostrar la flecha de seleccion
      lcd_putc(t[2]);
      
      if(unidad>14){// Si ya ingreso la contraseña muestra si es correcta o no, dependiendo si ingreso la clave correctamente.
         if(clave[0]==contrasena[0]&&clave[1]==contrasena[1]&&clave[2]==contrasena[2]&&clave[3]==contrasena[3]){
            lcd_gotoxy(1,1);
            printf(lcd_putc,"                    ");
            lcd_gotoxy(1,2);
            printf(lcd_putc,"     Contraseña     ");
            lcd_gotoxy(1,3);
            printf(lcd_putc,"      Correcta      ");
            lcd_gotoxy(1,4);
            printf(lcd_putc,"                    ");
            delay_ms(500);Menu=51;unidad=11;printf(lcd_putc,"\f");
            clave[0]=0;clave[1]=0;clave[2]=0;clave[3]=0;
         }else{
            lcd_gotoxy(1,1);
            printf(lcd_putc,"                    ");
            lcd_gotoxy(1,2);
            printf(lcd_putc,"     Contraseña     ");
            lcd_gotoxy(1,3);
            printf(lcd_putc,"     Incorrecta     ");
            lcd_gotoxy(1,4);
            printf(lcd_putc,"                    ");
            delay_ms(500);unidad=11;printf(lcd_putc,"\f");
            clave[0]=0;clave[1]=0;clave[2]=0;clave[3]=0;
         }
      }
               
      if(unidad>11&&unidad<14){
         lcd_gotoxy(unidad-1,4);// Para mostrar cursor.
         lcd_putc(t[1]);
      }
   }      
//----------------Fin-Menu5---------------------------------------------------------------

//----------------Menu5.1---------------------------------------------------------------
   if(Menu == 51){ // Menu cuando ingresa correctamente la contraseña, permite que digite nueva contraseña.
   
      lcd_gotoxy(1,1);
      printf(lcd_putc,"     Ingrese        ");
      lcd_gotoxy(1,2);
      printf(lcd_putc,"  Contraseña Nueva  ");
      lcd_gotoxy(1,3);
      printf(lcd_putc,"    CLAVE=%i%i%i%i  ",clave[0],clave[1],clave[2],clave[3]);
   
      
      if(UP){//Si oprime hacia arriba
         delay_ms(20);
         if(UP && Flanco == 0) {
            clave[unidad-11]++;Flanco = 1;delay_ms(30);
         }
      }else{
         Flanco = 0;
      }
            
      if(DOWN){//Si oprime hacia abajo
         delay_ms(20);
         if(DOWN && Flanco2 == 0) {
            clave[unidad-11]--;Flanco2 = 1;delay_ms(30);
         }
      }else{
         Flanco2 = 0;
      }
   
      if(RIGHT){// Si oprime Derecha
         delay_ms(20);
         if(RIGHT && Flanco1 == 0) {
            Flanco1 = 1;unidad++;delay_ms(30);printf(lcd_putc,"\f");
         }
      }else{
         Flanco1 = 0;
      }
            
      if(LEFT){// Si oprime Izquierda
         delay_ms(20);
         if(LEFT && Flanco3 == 0) {
            Flanco3 = 1;unidad--;delay_ms(30);printf(lcd_putc,"\f");
         }
      }else{
         Flanco3 = 0;
      }
            
      if(clave[unidad-11]<0)// Si la unidad donde se encuentra ubicado el cursor es menor que 0 pasa a 9.     
         clave[unidad-11]=9;
      if(clave[unidad-11]>9)// Si la unidad donde se encuentra ubicado el cursor es mayor que 9 pasa a 0.     
         clave[unidad-11]=0;
         
      if(unidad<11){
         delay_ms(500);Menu=1;paso=0;Flecha=2;Flecha2=2;
         clave[0]=0;clave[1]=0;clave[2]=0;clave[3]=0;
         printf(lcd_putc,"\f");unidad=11;
      }
      
      lcd_gotoxy(unidad,4);// Para mostrar la flecha de seleccion
      lcd_putc(t[2]);
      if(unidad>14){// Si ya ingreso la nueva contraseña.
         lcd_gotoxy(1,1);
         printf(lcd_putc,"                    ");
         lcd_gotoxy(1,2);
         printf(lcd_putc,"     Contraseña     ");
         lcd_gotoxy(1,3);
         printf(lcd_putc,"     Almacenada     ");
         lcd_gotoxy(1,4);
         printf(lcd_putc,"                    ");
         contrasena[0]=clave[0];contrasena[1]=clave[1];contrasena[2]=clave[2];contrasena[3]=clave[3];
         write_eeprom(0,clave[0]);delay_ms(20);write_eeprom(1,clave[1]);delay_ms(20);
         write_eeprom(2,clave[2]);delay_ms(20);write_eeprom(3,clave[3]);delay_ms(20);
         delay_ms(500);Menu=1;paso=0;Flecha=2;Flecha2=2;
         clave[0]=0;clave[1]=0;clave[2]=0;clave[3]=0;
         unidad=11;
      }
      
      if(unidad>11&&unidad<14){
         lcd_gotoxy(unidad-1,4);// Para mostrar cursor.
         lcd_putc(t[1]);
      }
   }      
//----------------Fin-Menu5.1---------------------------------------------------------------
#ifdef DAMPER
//----------------Menu6---------------------------------------------------------------
   if(Menu == 6){
      lcd_gotoxy(1,2);
      printf(lcd_putc,"     Media Movil    ");
      lcd_gotoxy(1,3);
      printf(lcd_putc,"       %02i   ",MediaMovil);
      
      if(MediaMovil>12)
         MediaMovil=12;
   
      if(MediaMovil<1)
         MediaMovil=1;
   
      if(UP){//Si oprime hacia arriba
         delay_ms(20);
         if(UP && Flanco == 0) {
            MediaMovil+=1;Flanco = 1;delay_ms(30);
         }
      }else{
         Flanco = 0;
      }
            
      if(DOWN){//Si oprime hacia abajo
         delay_ms(20);
         if(DOWN && Flanco2 == 0){
            MediaMovil-=1;Flanco2 = 1;delay_ms(30);
         }
      }else{
         Flanco2 = 0;
      }
   
      if(RIGHT){// Si oprime derecha
         delay_ms(20);
         if(RIGHT && Flanco1 == 0) {
            /*Flanco1 = 1;*/Menu=1;paso=0;Flecha=2;Flecha2=2;unidad=11;delay_ms(500);printf(lcd_putc,"\f");
            write_eeprom(42,MediaMovil);delay_ms(20);//Guardar valor de Setpoint en eeprom
            MensajeGuardado();
         }
      }else{
         Flanco1 = 0;
      }
            
      if(LEFT){// Si oprime Izquierda
         delay_ms(20);
         if(LEFT && Flanco3 == 0) {
            /*Flanco3 = 1;*/Menu=1;paso=0;Flecha=2;Flecha2=2;unidad=11;delay_ms(500);printf(lcd_putc,"\f");
         }
      }else{
         Flanco3 = 0;
      }
   }
//----------------Fin-Menu6---------------------------------------------------------------

//----------------Menu7---------------------------------------------------------------
   if(Menu == 7)// Menu de configuración de punto zero.
   {
      lcd_gotoxy(1,1);
      printf(lcd_putc,"  Zero Point Config ");
      lcd_gotoxy(1,2);
      printf(lcd_putc,"InFlow= %2.1f  (m/s) ",Flujo);
      lcd_gotoxy(1,3);
      printf(lcd_putc,"ZF=%2.0f ADC=%2.0f",zero_fabrica,sensores(0));
      lcd_gotoxy(1,4);
      printf(lcd_putc," Diferencia=%2.0f",Diferencia);
    
      if(RIGHT){// Si oprime derecha
         delay_ms(20);
         if(RIGHT && Flanco1 == 0) {
            /*Flanco1 = 1;*/Menu=1;paso=0;Flecha=2;Flecha2=2;unidad=11;delay_ms(500);printf(lcd_putc,"\f");
            Diferencia=zero_fabrica-sensores(0);
            
            if(Diferencia>=0){
               negativo=10;write_eeprom(50,negativo);delay_ms(20);
            }
            if(Diferencia<0){
               negativo=20;write_eeprom(50,negativo);delay_ms(20);
            }
            Diferencia=abs(Diferencia);
            Dif16=(int16)Diferencia;
            
            write_eeprom(60,make8(Dif16,0));delay_ms(20);
            write_eeprom(61,make8(Dif16,1));delay_ms(20);//Guardar valor de Setpoint en eeprom
            MensajeGuardado();
         }
      }else{
         Flanco1 = 0;
      }
            
      if(LEFT){// Si oprime Izquierda
         delay_ms(20);
         if(LEFT && Flanco3 == 0) {
            /*Flanco3 = 1;*/Menu=1;paso=0;Flecha=2;Flecha2=2;unidad=11;delay_ms(500);printf(lcd_putc,"\f");
         }
      }else{
         Flanco3 = 0;
      }
   }
//----------------Fin-Menu7---------------------------------------------------------------

//----------------Menu8---------------------------------------------------------------
   if(Menu == 8)// Menu de Mantenimiento
   {
      lcd_gotoxy(1,1);
      printf(lcd_putc," MANT.  V=%2.0f  ",sensores(0));//T= %2.0f",sensores(1));//T= %2.1f",TC);
      lcd_gotoxy(1,2);
      printf(lcd_putc,"Damp=%2.0f%%  T=%2.0f ",(100*((float)tmp/255)),sensores(1));
      lcd_gotoxy(1,3);
      printf(lcd_putc,"IF= %2.1f(m/s) Dam=%2.0f",Flujo,(100*(sensores(2)/1023)));
      lcd_gotoxy(1,4);
      printf(lcd_putc,"Ajuste1= %2.1f ",Ajuste1);
      
      if(Ajuste1>30.0)
         Ajuste1=0.0;
         
      if(Ajuste1<0.0)
         {Ajuste1=0.0;}
      
      if(UP){//Si oprime hacia arriba
         delay_ms(50);
         if(UP){
            Ajuste1+=0.1;
         }
      }
            
      if(DOWN){//Si oprime hacia abajo
         delay_ms(50);
         if(DOWN){
            Ajuste1-=0.1;
         }
      }
      
      if(RIGHT){// Si oprime derecha
         delay_ms(20);
         if(RIGHT && Flanco1 == 0) {
            /*Flanco1 = 1;*/Menu=1;paso=0;Flecha=2;Flecha2=2;unidad=11;delay_ms(30);printf(lcd_putc,"\f");
            Temporal=Ajuste1;
            Entero=(int)Ajuste1;
            Temporal=Temporal-Entero;
            Temporal2=Temporal*10.0;
            Decimal1=(int8)Temporal2;
            write_eeprom(25,Entero);delay_ms(20);
            write_eeprom(26,Decimal1);delay_ms(20);
            MensajeGuardado();
         }
      }else{
         Flanco1 = 0;
      }
            
      if(LEFT){// Si oprime Izquierda
         delay_ms(20);
         if(LEFT && Flanco3 == 0) {
            /*Flanco3 = 1;*/Menu=1;paso=0;Flecha=2;Flecha2=2;unidad=11;delay_ms(500);printf(lcd_putc,"\f");
         }
      }else{
         Flanco3 = 0;
      }
   }
//----------------Fin-Menu8---------------------------------------------------------------

//----------------Menu9---------------------------------------------------------------
   if(Menu == 9)// Menu de Modo
   {
   lcd_gotoxy(1,1);
   printf(lcd_putc," MODO   Damp=%2.0f%% ",(100*((float)tmp/255)));

   lcd_gotoxy(1,4);
   printf(lcd_putc,"IF= %2.1f(m/s) Dam=%2.0f",Flujo,(100*(sensores(2)/1023)));
      
         if(UP && DOWN){
            delay_ms(20);
            if(UP && DOWN){
               ModoAuto=0;
               ModoManual=1;
               ModoSemi=0;
               Automa=10;
               Semi=10;
               Manual=20;
            }
         }
         
         if(DOWN && RIGHT){
            delay_ms(20);
            if(DOWN && RIGHT){
               ModoAuto=1;
               ModoManual=0;
               ModoSemi=0;
               Automa=20;
               Semi=10;
               Manual=10;
            }
         }
         
         if(UP && RIGHT){
            delay_ms(20);
            if(UP && RIGHT){
               ModoAuto=0;
               ModoManual=0;
               ModoSemi=1;
               Automa=10;
               Semi=20;
               Manual=10;
            }
         }
      
      if(ModoManual == 1 || Manual==20)
      {
         ModoAuto=0;ModoSemi=0;Automa=10;Semi=10;
         lcd_gotoxy(1,2);
         printf(lcd_putc,"   Modo Manual  ");
         lcd_gotoxy(1,3);
         printf(lcd_putc,"    Seleccionado    ");
         if(LEFT)
         {
            delay_ms(20);
            if(LEFT){
               ModoSemi=0;ModoAuto=0;ModoManual=0;Manual=20;Automa=10;Semi=10;delay_ms(500);printf(lcd_putc,"\f");ajustar_damper(254);tmp=254; 
               write_eeprom(10,Manual);delay_ms(20);
               write_eeprom(12,Semi);delay_ms(20);
               write_eeprom(11,Automa);delay_ms(20);
               write_eeprom(13,tmp);delay_ms(20);
               MensajeGuardado();
               Menu=1;paso=0;Flecha=2;Flecha2=2;unidad=11;
            }
         } 
      }
        
      if(ModoAuto == 1 || Automa==20)
      {
         ModoManual=0;ModoSemi=0;Semi=10;Manual=10;
         lcd_gotoxy(1,2);
         printf(lcd_putc,"   Modo Auto   ");
         lcd_gotoxy(1,3);
         printf(lcd_putc,"    Seleccionado    ");
         if(LEFT)
         {
            delay_ms(20);
            if(LEFT){
               ModoSemi=0;ModoAuto=0;ModoManual=0;Automa=20;Semi=10;Manual=10;delay_ms(500);printf(lcd_putc,"\f");
               write_eeprom(10,Manual);delay_ms(20);
               write_eeprom(12,Semi);delay_ms(20);
               write_eeprom(11,Automa);delay_ms(20);
               write_eeprom(13,tmp);delay_ms(20);
               MensajeGuardado();
               Menu=1;paso=0;Flecha=2;Flecha2=2;unidad=11;
            }
         } 
      }
      
      if(ModoSemi == 1 || Semi==20)
      {
         ModoManual=0;ModoAuto=0;Automa=10;Manual=10;
         lcd_gotoxy(1,2);
         printf(lcd_putc,"   Modo Semi   ");
         lcd_gotoxy(1,3);
         printf(lcd_putc,"  Ajuste Valor    ");
         if(UP){
            delay_ms(20);
            if(UP)
               tmp++;
         }
         
         if(DOWN){
            delay_ms(20);
            if(DOWN)
               tmp--;
         }
         
         ValorSemi=tmp;
         ajustar_damper(tmp);
         if(LEFT)
         {
            delay_ms(20);
            if(LEFT){
               write_eeprom(13,tmp);delay_ms(20);ModoSemi=0;ModoAuto=0;ModoManual=0;Semi=20;Automa=10;Manual=10;delay_ms(500);printf(lcd_putc,"\f");
               write_eeprom(12,Semi);delay_ms(20);
               write_eeprom(11,Automa);delay_ms(20);
               write_eeprom(10,Manual);delay_ms(20);
               MensajeGuardado();
               Menu=1;paso=0;Flecha=2;Flecha2=2;unidad=11;
            }
         } 
      } 
   }
//----------------Fin-Menu9---------------------------------------------------------------

//----------------Menu10---------------------------------------------------------------
   if(Menu == 10)// Menu de Tiempo de Purga
   {
      lcd_gotoxy(1,1);
      printf(lcd_putc,"     Tiempo  de      ");
      lcd_gotoxy(1,2);
      printf(lcd_putc,"       Purga         ");
      lcd_gotoxy(6,3);
      printf(lcd_putc,"%02u:%02u (M:S)  ",purga_minutos_pro,purga_segundos_pro); 
      
      lcd_gotoxy(unidad2+6,4);// Para mostrar cursor.
      lcd_putc(t[2]);
      
      if(unidad2<1)
         unidad2=1;
      if(unidad2>3)
         unidad2=3;   
      
      if(UP){
         if(unidad2==1){
            if(purga_minutos_pro<61){
               purga_minutos_pro++;delay_ms(300);
            }
         }
         
         if(unidad2==3){
            if(purga_segundos_pro<60){
               purga_segundos_pro++;delay_ms(300);
            }
         }
      }
      
      if(DOWN){
         if(unidad2==1){
            if(purga_minutos_pro>1){
               purga_minutos_pro--;delay_ms(300);
            }
            
            if(purga_minutos_pro>60){
               purga_minutos_pro=60;delay_ms(300);
            }
         }
         
         if(unidad2==3){
            if(purga_segundos_pro>0){
               purga_segundos_pro--;delay_ms(300);
            }
            
            if(purga_segundos_pro>59){
               purga_segundos_pro=59;delay_ms(300);
            }
         }
      }
      
      if(RIGHT){
         if(unidad2==1){
            unidad2=3;
         }else{
            if(unidad2==3){
               unidad2=1;
            }else{
               unidad2=3;
            }
         }
         delay_ms(500);
         printf(lcd_putc,"\f");
      }
      
      if(LEFT)
      {
         delay_ms(200);
         printf(lcd_putc,"\f");
         lcd_gotoxy(1,2);
         printf(lcd_putc,"Tiempo Almacenado");
         lcd_gotoxy(1,3);
         printf(lcd_putc,"  Correctamente  ");
         write_eeprom(20,purga_minutos_pro);delay_ms(30);
         write_eeprom(21,purga_segundos_pro);delay_ms(30);
         purga_minutos=purga_minutos_pro;purga_segundos=purga_segundos_pro;
         delay_ms(700);
         delay_ms(30);Menu=1; paso=0;Flecha=2;Flecha2=2;printf(lcd_putc,"\f");
      }  
       
   }
//----------------Fin-Menu10---------------------------------------------------------------

//----------------Menu11---------------------------------------------------------------
   if(Menu == 11)// Menu de Tiempo de Post-Purga
   {
      lcd_gotoxy(1,1);
      printf(lcd_putc,"     Tiempo  de      ");
      lcd_gotoxy(1,2);
      printf(lcd_putc,"     Post-Purga      ");
      lcd_gotoxy(6,3);
      printf(lcd_putc,"%02u:%02u (M:S)  ",ppurga_minutos_pro,ppurga_segundos_pro); 
      
      lcd_gotoxy(unidad2+6,4);// Para mostrar cursor.
      lcd_putc(t[2]);
      if(unidad2<1)
         unidad2=1;
      if(unidad2>3)
         unidad2=3;   
      
      if(UP)
      {
         if(unidad2==1)
         {
            if(ppurga_minutos_pro<61)
            {
               ppurga_minutos_pro++;delay_ms(300);
            }
         }
         
         if(unidad2==3)
         {
            if(ppurga_segundos_pro<60)
            {
               ppurga_segundos_pro++;delay_ms(300);
            }
         }
      }
      
      if(DOWN)
      {
         if(unidad2==1)
         {
            if(ppurga_minutos_pro>1)
            {
               ppurga_minutos_pro--;delay_ms(300);
            }
            
            if(ppurga_minutos_pro>60)
            {
               ppurga_minutos_pro=60;delay_ms(300);
            }
         }
         
         if(unidad2==3)
         {
            if(ppurga_segundos_pro>0)
            {
               ppurga_segundos_pro--;delay_ms(300);
            }
            
            if(ppurga_segundos_pro>59)
            {
               ppurga_segundos_pro=59;delay_ms(300);
            }
         }
      }
      
      if(RIGHT)
      {
         if(unidad2==1)
         {
            unidad2=3;
         }
         else
         {
            if(unidad2==3)
            {
               unidad2=1;
            }
         }
         delay_ms(500);
         printf(lcd_putc,"\f");
      }
      
      if(LEFT)
      {
         delay_ms(200);
         printf(lcd_putc,"\f");
         lcd_gotoxy(1,2);
         printf(lcd_putc,"Tiempo Almacenado");
         lcd_gotoxy(1,3);
         printf(lcd_putc,"  Correctamente  ");
         write_eeprom(22,ppurga_minutos_pro);delay_ms(20);
         write_eeprom(23,ppurga_segundos_pro);delay_ms(20);
         ppurga_minutos=ppurga_minutos_pro;ppurga_segundos=ppurga_segundos_pro;
         delay_ms(700);
         delay_ms(30);Menu=1; paso=0;Flecha=2;Flecha2=2;printf(lcd_putc,"\f");
      }  
       
   }
//----------------Fin-Menu11---------------------------------------------------------------
#else

//----------------Menu6---------------------------------------------------------------
   if(Menu == 6)// Menu de Tiempo de Purga
   {
      lcd_gotoxy(1,1);
      printf(lcd_putc,"     Tiempo  de      ");
      lcd_gotoxy(1,2);
      printf(lcd_putc,"       Purga         ");
      lcd_gotoxy(6,3);
      printf(lcd_putc,"%02u:%02u (M:S)  ",purga_minutos_pro,purga_segundos_pro); 
      
      lcd_gotoxy(unidad2+6,4);// Para mostrar cursor.
      lcd_putc(t[2]);
      
      if(unidad2<1)
         unidad2=1;
      if(unidad2>3)
         unidad2=3;   
      
      if(UP){
         if(unidad2==1){
            if(purga_minutos_pro<61){
               purga_minutos_pro++;delay_ms(300);
            }
         }
         
         if(unidad2==3){
            if(purga_segundos_pro<60){
               purga_segundos_pro++;delay_ms(300);
            }
         }
      }
      
      if(DOWN){
         if(unidad2==1){
            if(purga_minutos_pro>1){
               purga_minutos_pro--;delay_ms(300);
            }
            
            if(purga_minutos_pro>60){
               purga_minutos_pro=60;delay_ms(300);
            }
         }
         
         if(unidad2==3){
            if(purga_segundos_pro>0){
               purga_segundos_pro--;delay_ms(300);
            }
            
            if(purga_segundos_pro>59){
               purga_segundos_pro=59;delay_ms(300);
            }
         }
      }
      
      if(RIGHT){
         if(unidad2==1){
            unidad2=3;
         }else{
            if(unidad2==3){
               unidad2=1;
            }else{
               unidad2=3;
            }
         }
         delay_ms(500);
         printf(lcd_putc,"\f");
      }
      
      if(LEFT)
      {
         delay_ms(200);
         printf(lcd_putc,"\f");
         lcd_gotoxy(1,2);
         printf(lcd_putc,"Tiempo Almacenado");
         lcd_gotoxy(1,3);
         printf(lcd_putc,"  Correctamente  ");
         write_eeprom(20,purga_minutos_pro);delay_ms(30);
         write_eeprom(21,purga_segundos_pro);delay_ms(30);
         purga_minutos=purga_minutos_pro;purga_segundos=purga_segundos_pro;
         delay_ms(700);
         delay_ms(30);Menu=1; paso=0;Flecha=2;Flecha2=2;printf(lcd_putc,"\f");
      }  
       
   }
//----------------Fin-Menu6---------------------------------------------------------------

//----------------Menu7---------------------------------------------------------------
   if(Menu == 7)// Menu de Tiempo de Post-Purga
   {
      lcd_gotoxy(1,1);
      printf(lcd_putc,"     Tiempo  de      ");
      lcd_gotoxy(1,2);
      printf(lcd_putc,"     Post-Purga      ");
      lcd_gotoxy(6,3);
      printf(lcd_putc,"%02u:%02u (M:S)  ",ppurga_minutos_pro,ppurga_segundos_pro); 
      
      lcd_gotoxy(unidad2+6,4);// Para mostrar cursor.
      lcd_putc(t[2]);
      if(unidad2<1)
         unidad2=1;
      if(unidad2>3)
         unidad2=3;   
      
      if(UP)
      {
         if(unidad2==1)
         {
            if(ppurga_minutos_pro<61)
            {
               ppurga_minutos_pro++;delay_ms(300);
            }
         }
         
         if(unidad2==3)
         {
            if(ppurga_segundos_pro<60)
            {
               ppurga_segundos_pro++;delay_ms(300);
            }
         }
      }
      
      if(DOWN)
      {
         if(unidad2==1)
         {
            if(ppurga_minutos_pro>1)
            {
               ppurga_minutos_pro--;delay_ms(300);
            }
            
            if(ppurga_minutos_pro>60)
            {
               ppurga_minutos_pro=60;delay_ms(300);
            }
         }
         
         if(unidad2==3)
         {
            if(ppurga_segundos_pro>0)
            {
               ppurga_segundos_pro--;delay_ms(300);
            }
            
            if(ppurga_segundos_pro>59)
            {
               ppurga_segundos_pro=59;delay_ms(300);
            }
         }
      }
      
      if(RIGHT)
      {
         if(unidad2==1)
         {
            unidad2=3;
         }
         else
         {
            if(unidad2==3)
            {
               unidad2=1;
            }
         }
         delay_ms(500);
         printf(lcd_putc,"\f");
      }
      
      if(LEFT)
      {
         delay_ms(200);
         printf(lcd_putc,"\f");
         lcd_gotoxy(1,2);
         printf(lcd_putc,"Tiempo Almacenado");
         lcd_gotoxy(1,3);
         printf(lcd_putc,"  Correctamente  ");
         write_eeprom(22,ppurga_minutos_pro);delay_ms(20);
         write_eeprom(23,ppurga_segundos_pro);delay_ms(20);
         ppurga_minutos=ppurga_minutos_pro;ppurga_segundos=ppurga_segundos_pro;
         delay_ms(700);
         delay_ms(30);Menu=1; paso=0;Flecha=2;Flecha2=2;printf(lcd_putc,"\f");
      }  
       
   }
//----------------Fin-Menu7---------------------------------------------------------------
#endif
//-------------------Menu20----------------------------------------------------------------  

   if(Menu==20){
      if(Lectura2==1){
         /*if(imprimio){
            letra=lcd_getc(1,1);
            delay_ms(1);
            if(letra!='J')
               Ruido=1;
         }*/
         lcd_gotoxy(1,1);                     
         printf(lcd_putc," JP CEGH ");//T= %2.0f",sensores(1));//T= %2.1f",TC);
         
         #ifdef DAMPER
         lcd_gotoxy(1,2);
         printf(lcd_putc,"Ext=%2.0f%%",(100*((float)tmp/255)));
         //printf(lcd_putc,"Ext= %03u%%",FeedbackDamper);
         lcd_gotoxy(1,3);
         printf(lcd_putc,"InFlow= %2.1f (m/s) ",Flujo);
         if(Flujo<0.5){
            lcd_gotoxy(11,2);
            printf(lcd_putc,"Flujo=Bajo");      
         }else{
            if(Flujo>2.0){
               lcd_gotoxy(11,2);
               printf(lcd_putc,"Flujo=Alto");               
            }else{
               lcd_gotoxy(11,2);
               printf(lcd_putc,"Flujo=OK  ");      
            }
         } 
         #else
         lcd_gotoxy(1,2);
         printf(lcd_putc,"Ext= ---");
         lcd_gotoxy(1,3);
         printf(lcd_putc,"InFlow= ---   ");
         lcd_gotoxy(11,2);
         printf(lcd_putc,"Flujo=--- ");
         #endif
         imprimio=1;
         Lectura2=0;
      }
      
      if(flag_filtro==1){
         if(horas>=60){
            lcd_gotoxy(10,1);                     
            printf(lcd_putc,"Filtro:CAM");
         }else{
            lcd_gotoxy(10,1);                     
            printf(lcd_putc,"Filtro:OK ");
         }
      }else{
         lcd_gotoxy(10,1);                     
         printf(lcd_putc,"Filtro:---");
      }
      
      if(estadofl==1){
         Luz_Blanca_on;
         lcd_gotoxy(1,4);
         printf(lcd_putc,"Luz:ON ");
      }else{
         Luz_Blanca_off;
         lcd_gotoxy(1,4);
         printf(lcd_putc,"Luz:OFF ");
      }
      
      if(estadomv==1){
         lcd_gotoxy(10,4);
         printf(lcd_putc,"Motor:ON ");
      }else{
         if(iniciodamper==1){
            //Motor_off;     
            ApagaMotor();
         }
         lcd_gotoxy(10,4);
         printf(lcd_putc,"Motor:OFF "); 
      }
      
      if(DOWN){//Si oprime hacia Abajo
         delay_ms(20);
         if(DOWN && Flanco == 0){
            Flanco = 1;delay_ms(200);estadofl=!estadofl;
         }
      }else{
         Flanco = 0;
      }
            
      if(LEFT){//Si oprime hacia Izquierda
         delay_ms(20);
         if(LEFT && Flanco2 == 0){
            Flanco2 = 1;delay_ms(200);estadomv=!estadomv;
            if(estadomv==1){
               //Motor_on;
               EnciendeMotor();
               delay_ms(150);
               lcd_init();
            }else{
               //Motor_off;
               ApagaMotor();
               delay_ms(150);
               lcd_init();
            }
         }
      }else{
         Flanco2 = 0;
      }
   
      if(UP){// Si oprime Arriba
         delay_ms(20);
         if(UP && Flanco1 == 0) {
            /*Flanco1 = 1;*/delay_ms(500);printf(lcd_putc,"\f");Menu=1;paso=0;Flecha=2;Flecha2=2;unidad=11;
         }
      }else{
         Flanco1 = 0;
      }
            
      if(RIGHT){// Si oprime Derecha
         delay_ms(20);
         if(RIGHT && Flanco3 == 0){
            delay_ms(500);printf(lcd_putc,"\f");Menu=40;
         }
      }else{
         Flanco3 = 0;
      }
   }
//----------------Fin-Menu20---------------------------------------------------------------  

//-----------------Menu30------------------------------------------------------------------  
   while(Menu == 30)
   {
      lcd_gotoxy(1,1);
      printf(lcd_putc,"    !Post-Purga!    ");
      lcd_gotoxy(1,2);
      printf(lcd_putc,"                    ");
      lcd_gotoxy(1,3);
      printf(lcd_putc,"      %02u:%02u  ",ppurga_minutos,ppurga_segundos); 
          
      if(Lectura==1)
      {
         Luz_Blanca_off;
         
         if(sensores(2)<6 && ppurga_segundos==0 && ppurga_minutos==0)
         {
            write_eeprom(70,segundos);delay_ms(20);
            write_eeprom(71,minutos);delay_ms(20);
            write_eeprom(72,horas);delay_ms(20);
            printf(lcd_putc,"\f");
            lcd_gotoxy(1,2);
            printf(lcd_putc,"     Proceso      ");
            lcd_gotoxy(1,3);
            printf(lcd_putc,"    Finalizado    ");
            delay_ms(2000);
            printf(lcd_putc,"\f");
            ajustar_damper(0);
            reset_cpu();
         }
         Lectura=0;
      }
      
   }

//----------------Fin-Menu30---------------------------------------------------------------  

//----------------Menu40---------------------------------------------------------------
   if(Menu == 40){ // Menu de apagado de cabina
   lcd_gotoxy(1,1);
   printf(lcd_putc,"   Desea apagar     ");
   lcd_gotoxy(1,2);
   printf(lcd_putc,"     la cabina      ");
   lcd_gotoxy(1,3);
   printf(lcd_putc,"   > Aceptar        ");
   lcd_gotoxy(1,4);
   printf(lcd_putc,"   < Cancelar       ");
  
   
      if(RIGHT){// Si oprime derecha
         delay_ms(20);
         if(RIGHT && Flanco1 == 0){
            Flanco1 = 1;Menu=30;intensidadpantalla(contraste);Alarma_off;printf(lcd_putc,"\f");tiempo_ppurga=1;
         }
      }else{
         Flanco1 = 0;
      }
            
      if(LEFT){// Si oprime Izquierda
         delay_ms(20);
         if(LEFT && Flanco3 == 0){
            Flanco3 = 1;Menu=20;paso=0;Flecha=2;Flecha2=2;unidad=11;delay_ms(500);printf(lcd_putc,"\f");
         }
      }else{
         Flanco3 = 0;
      }
   }      
//----------------Fin-Menu40---------------------------------------------------------------   

//----------------Menu50---------------------------------------------------------------
   if(Menu == 50){ // Menu de Purga
      lcd_gotoxy(1,1);
      printf(lcd_putc,"      !Purga!       ");
      lcd_gotoxy(1,2);
      printf(lcd_putc,"                    ");
      lcd_gotoxy(1,3);
      printf(lcd_putc,"      %02u:%02u   ",purga_minutos,purga_segundos); 
       
      if(LEFT){
         delay_ms(1000);
         if(LEFT){
            tiempo_purga=0;
            EnciendeMotor();//Motor_on;
            Menu=20;
         }
      }
      if(Lectura==1){
        Flujo=Leer_Sensor_Flujo(MediaMovil);
        controlar_flujo();    
        //Motor_on;
        EnciendeMotor();
        Luz_Blanca_off;
      }
   }      
//----------------Fin-Menu50---------------------------------------------------------------  

   if(tiemporeset>=60)
   {
      lcd_init();
      tiemporeset=0;
   } 

   #ifdef DAMPER
   if(Menu!=0 || Menu!=30)
   {
      if(Lectura==1)
      {
         FeedbackDamper=(int8)(100*(sensores(2)/1023));
         Flujo=Leer_Sensor_Flujo(MediaMovil);
         controlar_flujo();
         
         if(Flujo<0.5 && Menu==20)
         {
              flag_latencia=1;
              
              if(Latencia==1)
              {
                 if(estadoalarma==1)
                 {
                    Alarma_on;
                 }
                 else
                 {
                    Alarma_off;
                 }
                 estadopantalla=!estadopantalla;
                 if(estadopantalla==1)
                 {intensidadpantalla(contraste);}
                 else
                 {intensidadpantalla(0);}
               }
            }
         else
         {
            intensidadpantalla(contraste);Alarma_off;
            if(Flujo>=0.5)
            {
               Latencia=0;t_latencia=0;flag_latencia=0;
            }
         }
      }
      Lectura=0;
   }   
   else
   {
      intensidadpantalla(contraste);Alarma_off;Latencia=0;t_latencia=0;flag_latencia=0;
   }
   #else
   if(Menu!=0 || Menu!=30 || Menu!=50)
   {
      if(Lectura==1)
      {
         
         if(horas>=setpoint && Menu==20)
         {
             if(estadoalarma==1)
             {
                Alarma_on;
             }
             else
             {
                Alarma_off;
             }
             estadopantalla=!estadopantalla;
             if(estadopantalla==1)
             {intensidadpantalla(contraste);}
             else
             {intensidadpantalla(0);}
         }
         else
         {
            intensidadpantalla(contraste);Alarma_off;
         }
         Lectura=0;
      }
      //Lectura=0;
   }   
   else
   {
      intensidadpantalla(contraste);Alarma_off;
   }
   #endif
   if(Ruido){
//      Refresco();
      imprimio=0;
   }

   }       
}

