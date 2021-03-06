////////////////////////////////////////////////////////////////////////////
////        (C) Copyright 2018 JP Bioingenieria SAS                     ////
////              JUDAPIES-dielecingenieria.com                         ////
////////////////////////////////////////////////////////////////////////////

#include <18F4550.h>
#device adc=10
#define SIMU 1
#ifdef SIMU
#fuses HSPLL,NOWDT,NOPROTECT,NOLVP,NODEBUG,USBDIV,PLL5,CPUDIV4,VREGEN,MCLR// PLL1 para 4 MHz
#use delay(clock=16000000)
#else
#fuses HSPLL,NOWDT,NOPROTECT,NOLVP,NODEBUG,USBDIV,PLL5,CPUDIV1,VREGEN,MCLR// PLL1 para 4 MHz
#use delay(clock=48000000)
#define FAST 1
#endif

//#define use_portd_lcd TRUE
//#include <LCD420PIC18F.c>
#include <LCD420.c>
#include <math.h>

//#define DAMPER 1
#define VARIADOR 1
#define FLUJOS 1

// Definición de teclado - NO
#define   RIGHT         !input(PIN_E1)
#define   LEFT          !input(PIN_E2)
#define   DOWN          !input(PIN_E0)
#define   UP            !input(PIN_A5)
#define   ALARMA        !input(PIN_A3)

// Definición de otros puertos

#define   Luz_Blanca_on     output_bit(PIN_B4,1)
#define   Luz_Blanca_off    output_bit(PIN_B4,0)
#define   Motor_on          output_bit(PIN_B5,1)
#define   Motor_off         output_bit(PIN_B5,0)
#define   Aux_on            output_bit(PIN_B6,1)
#define   Aux_off           output_bit(PIN_B6,0)
#define   Alarma_on         output_bit(PIN_B7,1)
#define   Alarma_off        output_bit(PIN_B7,0)
#define   Pantalla_on       output_bit(PIN_C7,1)
#define   Pantalla_off      output_bit(PIN_C7,0)

int8 v=0;
float vel=0.0;
int8 Menu=100,n_opcionL=2,unidad=11,Flecha=2,Flecha1=3,Flecha2=2, nMenuL=2, nMenuH=4,contraste=10,unidad2=1,purga_minutos=0,purga_minutos_pro=0;
int8 Entero=0,Decimal1=0,Entero1=0,l=0,l2=0,h=0,h2=0,tmp=10,MediaMovil=0,negativo=10,Automa=10,Manual=10,Semi=10,ValorSemi=0,Opcion=1;
int8 segundos=0,minutos=0,horas=0,tiemporeset=0,purga_segundos=0,purga_segundos_pro=0,ppurga_segundos=0,ppurga_segundos_pro=0,ppurga_minutos=0,ppurga_minutos_pro=0;
int8 setpoint=0;
short estadofl=0,estadomv=0,estadoalarma=0,estadopantalla=0,Flanco=0,Flanco1=0,Flanco2=0,Flanco3=0,flag_latencia=0,Latencia=0,flag_filtro=0;
short imprimio=0,Ruido=0;
signed int8   paso=0;
char t[3]={'>',' ','^'}; 
signed  int8 clave[4]={0,0,0,0};   // 0=Verdadero, 1=Falso
signed int8 contrasena[4]={0,0,0,0};   // 0=Verdadero, 1=Falso
float zero_fabrica=203.0;
short Lectura=0,Lectura2=0,flag_iniciodamper=0,ModoManual=0,ModoAuto=0,ModoSemi=0,tiempo_purga=0,tiempo_ppurga=0;
int16 tiempos=0,t_latencia=0,tiempos2=0,Dif16=0;
//float a=24.9616,b=-175.3954,c=489.5268,d=-440.7857;
//float a=-0.00000007109886647647915,b=0.0003576135181122329,c= -0.169922240146291,d= 19.248973132140719;//Sensor 1 Posible dańado
float a=0.0000001418633343546420,b=-0.00009476749001431169,c= 0.040182822903506,d= -5.354852229527197;//Sensor 2 Posible bueno
float Flujo=0.0,Flujo2=0.0,x=0.0,x_uno=0.0,x_cuadrado=0.0,x_cubo=0.0,Temporal=0.0,Temporal2=0.0,Velocidad=0.0;
float Temporal1=0.0,Diferencia=0.0,Ajuste1=1.0;
float error=0.0,Temperatura=0;
float promedio[12]={0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
float promedio2[12]={0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};

int8 n_opcionH=8;

#include <Funciones.h>
#include <Menu.h>

#int_TIMER1
void temp1s(void){
   #ifdef SIMU
   set_timer1(45536);  // 5  ms
   #else
   set_timer1(5536);  // 5  ms
   #endif
   tiempos++;
   tiempos2++;
   
   if(t_latencia>=3000)
      Latencia=1;
   
   if(flag_latencia==1)
      t_latencia++;         
   
   if(tiempos2>=200){
      tiempos2=0;Lectura2=1;
   }
   
   if(tiempos>=200){   // Ha transcurrido una decima de segundo (PIC18F4550 con XT = 16MHZ)
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
               Aux_on;
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
   delay_ms(1000);
   ConfigInicial();
   Menu=200;
   SalidasOff();
   while(true){

   while(Menu==200){
      SalidasOff();
      if(RIGHT){
         delay_ms(1000);
         if(RIGHT){   
            LeeEEPROM();            
            LimitaValores(); 
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
            printf(lcd_putc,"\f");
            delay_ms(500);
         }
      }
   }

//------------Menu0------------------------------------------------------------------   
    while(Menu == 0){ // Menu de Contraseńa para Poder iniciar el equipo
      flag_latencia=1;
      SalidasOff();
      if(Latencia==1){
         printf(lcd_putc,"\f");reset_cpu();
      }
      
      if(SolicitaContrasena(0,0,50,contrasena[0],contrasena[1],contrasena[2],contrasena[3])==1){
         printf(lcd_putc,"\f");flag_latencia=0;Latencia=0;tiempo_purga=1;
         t_latencia=0;flag_iniciodamper=1; 
      }
   }
   
   if(Menu == 1){ // Menu de seleccion de lo que desea encender
      MenuPrincipal();
   }

   if(Menu == 2){ // Menu de seleccion de estado de Alarma
      MenuAlarma();
   }      

   if(Menu == 3){ // Menu de Configuracion de Tiempos de Filtro
      MenuFiltro();  
   }
   
   if(Menu == 4){ // Menu de Configuracion de Tiempo de vida de filtro
      MenuSetpoint();  
   }
   
   if(Menu == 5){ // Menu para Cambio de Contraseńa
      SolicitaContrasena(5,1,51,contrasena[0],contrasena[1],contrasena[2],contrasena[3]);
   }      

   if(Menu == 51){ // Menu cuando ingresa correctamente la contraseńa, permite que digite nueva contraseńa.
      MenuCambioClave();
   }      
   
   if(Menu == 6){
      MenuTiempoPurga();
   }
   
   if(Menu == 7){
      MenuTiempoPostPurga();
   }
   
   if(Menu == 8){
      while(SolicitaContrasena(8,1,71,4,4,4,4)==0 && Menu==8){}
      clave[0]=0;clave[1]=0;clave[2]=0;clave[3]=0;
   }
   
   if(Menu == 71){
      MenuServicio();
   }

   if(Menu == 9){
      MenuMedia();
   }

   if(Menu == 10){// Menu de configuración de punto zero.   
      MenuPuntoCero();
   }

   if(Menu == 11){// Menu de Mantenimiento
      MenuMantenimiento();
   }
   
   #ifdef DAMPER
   if(Menu == 12){// Menu de Modo
      MenuModo();
   }
   #endif
   
   #ifdef VARIADOR
   if(Menu == 12){// Menu de Modo
      MenuVelocidad();
   }
   #endif

//-------------------Menu20----------------------------------------------------------------  
   if(Menu==20){
      
      if(Lectura2==1){
         
         lcd_gotoxy(1,1);                     
         printf(lcd_putc,"JP CEGH");//T= %2.0f",sensores(1));//T= %2.1f",TC);
        
         #ifdef VARIADOR
         lcd_gotoxy(1,2);
         vel=((float)tmp/254)*100;
         printf(lcd_putc,"Ext:%03i%%",(int8)vel);
         #endif
         
         #ifdef DAMPER
         lcd_gotoxy(1,2);
         printf(lcd_putc,"Ext:%2.0f%%",(100*((float)tmp/255)));         
         #endif
         
         #ifdef FLUJOS
         lcd_gotoxy(1,3);
         printf(lcd_putc,"InFlow: %2.1f (m/s) ",Flujo);
         
         if(Flujo<0.5){
            lcd_gotoxy(11,2);
            printf(lcd_putc,"Flujo:Bajo");      
         }else{
            if(Flujo>2.0){
               lcd_gotoxy(11,2);
               printf(lcd_putc,"Flujo:Alto");               
            }else{
               lcd_gotoxy(11,2);
               printf(lcd_putc,"Flujo:OK  ");      
            }
         }
         #else
         lcd_gotoxy(1,3);
         printf(lcd_putc,"Temperatura: %2.0f C",leeTemperatura(MediaMovil));
         if(ALARMA){
            delay_ms(200);
            if(ALARMA){
               estadomv=0;
               estadofl=1;
               lcd_gotoxy(10,2);
               printf(lcd_putc,"Alarma:EV1");
            }
         }else{
            lcd_gotoxy(10,2);
            printf(lcd_putc,"Alarma:---");
         }
         #endif
         imprimio=1;
         Lectura2=0;
      }
      
      if(flag_filtro==1){
         if(horas>=setpoint){
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
         Aux_on;
         lcd_gotoxy(1,4);
         printf(lcd_putc,"Luz:ON ");
      }else{
         Luz_Blanca_off;
         Aux_off;
         lcd_gotoxy(1,4);
         printf(lcd_putc,"Luz:OFF ");
      }
      
      if(estadomv==1){
         lcd_gotoxy(10,4);
         printf(lcd_putc,"Motor:ON ");
         #ifdef VARIADOR
         ajuste_vel(tmp);
         #endif
      }else{
         #ifdef DAMPER
         if(iniciodamper==1){
            //Motor_off;     
            ApagaMotor();
         }
         #else
         ApagaMotor();
         #endif
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

   if(Menu == 30){
      MenuPostPurga();
   }

   if(Menu == 40){ // Menu de apagado de cabina
      MenuApagado();
   }      

   if(Menu == 50){ // Menu de Purga
      MenuPurga();
   }      

   if(tiemporeset>=60){
      lcd_init();
      tiemporeset=0;
   } 

   if(Menu!=0 || Menu!=30){
      if(Lectura==1){
         Flujo=Leer_Sensor_Flujo(MediaMovil);
         Temperatura=leeTemperatura(MediaMovil);
         #ifdef DAMPER
         controlar_flujo();
         #endif
         
         #ifdef FLUJOS
         if(Flujo<0.5 && Menu==20){
              flag_latencia=1; 
              if(Latencia==1){
                 if(estadoalarma==1){
                    //Alarma_on;
                    output_toggle(PIN_B7);
                    estadopantalla=!estadopantalla;
                 }else{
                    Alarma_off;
                    estadopantalla=1;
                 }
                 if(estadopantalla==1)
                  intensidadpantalla(contraste);
                 else
                  intensidadpantalla(0);
               }
            }
         else{
            intensidadpantalla(contraste);Alarma_off;
            if(Flujo>=0.5){
               Latencia=0;t_latencia=0;flag_latencia=0;
            }
         }
         #else
         if(estadomv==0 && estadofl==1 && Menu==20){
             if(estadoalarma==1){
                //Alarma_on;
                output_toggle(PIN_B7);
             }else{
                Alarma_off;
             }
             estadopantalla=!estadopantalla;
             if(estadopantalla==1)
              intensidadpantalla(contraste);
             else
              intensidadpantalla(0);
         }else{
            intensidadpantalla(contraste);Alarma_off;
         }
         #endif
      }
      Lectura=0;
   }else{
      intensidadpantalla(contraste);Alarma_off;Latencia=0;t_latencia=0;flag_latencia=0;
   }
   
   if(Ruido){
//      Refresco();
      imprimio=0;
   }

   }       
}

