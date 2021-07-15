// Cuatro pulsadores de entrada (Derecha, Izquierda, arriba y abajo).
// Reloj con XT de 4 MHz.
// Programación para Cabina Extractora de Gases y humos.
// Tiene Contraseña de incio para permitir funcionamiento de Cabina.
// Tiene Menú:Luz Blanca, Motor Ventilador, Intensidad de Pantalla, Alarma y Cambio de Contraseña.
// Tiene Lector de velocidad de aire.
// Tiene control de velocidad de aire por Damper.
// Ing. Juan David Piñeros.
// JP Inglobal.
//* Lectura de sensor de vidrio.

#include <18F4550.h>
#device adc=10
//#fuses XT,NOWDT,NOPROTECT,NOLVP,NODEBUG,VREGEN,NOMCLR// PLL1 para 4 MHz
//#use delay(clock=4000000)
#fuses HSPLL,NOWDT,NOPROTECT,NOLVP,NODEBUG,USBDIV,PLL1,CPUDIV1,NOVREGEN,MCLR// PLL1 para 4 MHz
#use delay(clock=48000000)
#include <LCD420.c>
#include <math.h>

// Definición de teclado - NC
#define   RIGHT         input(PIN_B0)
#define   LEFT          input(PIN_B1)
#define   DOWN          input(PIN_B2)
#define   UP            input(PIN_B3)

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

signed int8 Menu=100, n_opcionH=7,n_opcionL=2,unidad=11,Flecha=2,Flecha1=3,Flecha2=2, nMenuL=2, nMenuH=4,contraste=10,unidad2=1,purga_minutos=0,purga_minutos_pro=0;
signed int8 Entero=0,tmp=10,Automa=10,Manual=10,Semi=10,ValorSemi=0;
signed int8 segundos=0,minutos=0,horas=0,tiemporeset=0,purga_segundos=0,purga_segundos_pro=0,ppurga_segundos=0,ppurga_segundos_pro=0,ppurga_minutos=0,ppurga_minutos_pro=0;
short estadofl=0,estadomv=0,estadoalarma=0,Flanco=0,Flanco1=0,Flanco2=0,Flanco3=0,flag_latencia=0,Latencia=0,flag_filtro=0,estadopantalla=0;
signed int8   paso=0;
char t[3]={'>',' ','^'}; 
signed  int8 clave[4]={0,0,0,0};   // 0=Verdadero, 1=Falso
int8 contrasena[4]={0,0,0,0};   // 0=Verdadero, 1=Falso
float setpoint=0.0,Temporal=0.0;
short ModoSleep,inicio=0,Lectura=0,Lectura2=0,flag_iniciodamper=0,iniciodamper=0,tiempo_purga=0,tiempo_ppurga=0;
int16 tiempos=0,t_latencia=0,tiempos2=0;


// Funcion para conversor analogo-digital
float sensores(int x){
float y;set_adc_channel(x);delay_ms(10);y=read_adc();return (y);
}

void intensidadpantalla(int x)
{
   if(x==0)
   {
      Pantalla_off;
   }
   else
   {
      Pantalla_on;
   }
}

#int_TIMER1
void temp1s(void){
   
   set_timer1(5536);  // 5  ms
   tiempos++;
   tiempos2++;
    
   if(tiempos2>=20)
   {tiempos2=0;Lectura2=1;}
   
   if(tiempos>=200)   // Ha transcurrido una decima de segundo (PIC18F4550 con XT = 16MHZ)
   {
      tiempos=0;tiemporeset++;
      Lectura=1;
         
      if(estadomv==1 && flag_filtro==1)
      {  
         segundos++;
         if(segundos>=60)
         {
            segundos=0;minutos++;
         }
         
         if(minutos>=60)
         {
            minutos=0;horas++;
         }
      }
      
      if(tiempo_purga==1)
      {
         if(purga_segundos>0)
         {
            purga_segundos--;
         }
         else
         {
            if(purga_minutos>0)
            {
               purga_minutos--;
               purga_segundos=59;
            }
            else
            {
               //tiempo_cumplido=1;
               tiempo_purga=0;Motor_on;Menu=20;
               Luz_Blanca_on;
               estadomv=1;
               estadofl=1;
            }
         }
      }
      
      if(tiempo_ppurga==1)
      {
         if(ppurga_segundos>0)
         {
            ppurga_segundos--;
         }
         else
         {
            if(ppurga_minutos>0)
            {
               ppurga_minutos--;
               ppurga_segundos=59;
            }
            else
            {
               //tiempo_cumplido=1;
               reset_cpu();
            }
         }
      }
   }
}

void mensajes(int8 x,y)// Funcion para imprimir mensajes de Menu Principal.
{    
   if(x==1)
   {if(estadoalarma==0)
      {lcd_gotoxy(2,y);printf(lcd_putc,"Alarma          OFF");}
   if(estadoalarma==1)
      {lcd_gotoxy(2,y);printf(lcd_putc,"Alarma           ON");}
   }
   
   if(x==2)
   {lcd_gotoxy(2,y);printf(lcd_putc,"Filtro             ");}
   
   if(x==3)
   {lcd_gotoxy(2,y);printf(lcd_putc,"Vida Filtro        ");}
   
   if(x==4)
   {lcd_gotoxy(2,y);printf(lcd_putc,"Cambio Contraseña  ");}
      
   if(x==5)
   {lcd_gotoxy(2,y);printf(lcd_putc,"Tiempo de Purga    ");}
   
   if(x==6)
   {lcd_gotoxy(2,y);printf(lcd_putc,"Tiempo Post-Purga  ");}
}


void main ()
{
   setup_adc_ports(AN0_TO_AN3);
   setup_adc(ADC_CLOCK_DIV_32 );
   output_d(0); 
   ModoSleep=1;
   setup_timer_1(T1_INTERNAL|T1_DIV_BY_1);
   enable_interrupts(INT_TIMER1);
   setup_timer_2(T2_DIV_BY_4, 255, 1); 
   setup_ccp1 (CCP_PWM);
   enable_interrupts(GLOBAL);
   set_pwm1_duty(0);
   lcd_init();
   printf(lcd_putc,"\f");
            
   // Lee la contraseña almacenada en la eeprom para poder comprobar con la que escribe el usuario.
   contrasena[0]=read_eeprom(0);delay_ms(20); contrasena[1]=read_eeprom(1);delay_ms(20);
   contrasena[2]=read_eeprom(2);delay_ms(20); contrasena[3]=read_eeprom(3);delay_ms(20);
   estadoalarma=read_eeprom(5);
   delay_ms(20);
   Manual=read_eeprom(10);
   delay_ms(20);
   Automa=read_eeprom(11);
   delay_ms(20);
   Semi=read_eeprom(12);
   delay_ms(20);
   ValorSemi=read_eeprom(13);
   delay_ms(20);
   tmp=ValorSemi;

   purga_minutos=read_eeprom(20);
   delay_ms(20);
   purga_segundos=read_eeprom(21);
   delay_ms(20);
   purga_minutos_pro=purga_minutos;purga_segundos_pro=purga_segundos;
   ppurga_minutos=read_eeprom(22);
   delay_ms(20);
   ppurga_segundos=read_eeprom(23);
   ppurga_minutos_pro=ppurga_minutos;ppurga_segundos_pro=ppurga_segundos;
         
   Entero=read_eeprom(40);//Decimal2=read_eeprom(41);
   setpoint=Entero;
   delay_ms(20);
   segundos=read_eeprom(70);
   delay_ms(20);
   minutos=read_eeprom(71);
   delay_ms(20);
   horas=read_eeprom(72);
   delay_ms(20);
   flag_filtro=read_eeprom(73);
   Menu=200;
  
   while(true){

   while(Menu==200)
   {
      if(inicio==0)
      {
      delay_ms(500);
         if(input(PIN_B0))
         {
            lcd_init();
            intensidadpantalla(contraste);
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
            inicio=1;
            Menu=0;
            ModoSleep=0;
            delay_ms(500);
         }
      }
   }

//------------Menu0------------------------------------------------------------------   
   while(Menu == 0){ // Menu de Contraseña para Poder iniciar el equipo
   flag_latencia=1;set_pwm1_duty(0);
      if(Latencia==1)
         {
            printf(lcd_putc,"\f");reset_cpu();
         }
   lcd_gotoxy(1,1);
   printf(lcd_putc,"     Ingrese        ");
   lcd_gotoxy(1,2);
   printf(lcd_putc,"    Contraseña      ");
   lcd_gotoxy(1,3);
   printf(lcd_putc,"    CLAVE=%i%i%i%i  ",clave[0],clave[1],clave[2],clave[3]);
   
      
   if(UP)//Si oprime hacia arriba
      {  if(Flanco == 0) 
            {clave[unidad-11]++;Flanco = 1;delay_ms(30);t_latencia=0;}}
         else
            {Flanco = 0;}
            
      if(DOWN)//Si oprime hacia abajo
      {  if(Flanco2 == 0) 
            {clave[unidad-11]--;Flanco2 = 1;delay_ms(30);t_latencia=0;}}
         else
            {Flanco2 = 0;}
   
      if(RIGHT)// Si Oprime Derecha
      {  if(Flanco1 == 0) 
            {Flanco1 = 1;unidad++;delay_ms(30);printf(lcd_putc,"\f");t_latencia=0;}}
         else
            {Flanco1 = 0;}   
            
       if(LEFT)// Si Oprime izquierda
      {  if(Flanco3 == 0) 
            {Flanco3 = 1;unidad--;delay_ms(30);printf(lcd_putc,"\f");t_latencia=0;}}
         else
            {Flanco3 = 0;}    
            
       if(clave[unidad-11]<0)     // Si la unidad donde se encuentra ubicado el cursor es menor que 0 pasa a 9.
       {clave[unidad-11]=9;}
       if(clave[unidad-11]>9)     // Si la unidad donde se encuentra ubicado el cursor es mayor que 9 pasa a 0.
       {clave[unidad-11]=0;}
       if(unidad<11)             // Si trata de correr mas a la izquierda de la primera unidad, deja el cursor en esa posicion.
       {unidad=11;}
       lcd_gotoxy(unidad,4);// Para mostrar cursor.
       lcd_putc(t[2]);
       if(unidad>14)             // Si a Terminado de ingresar la clave, verifica si es correcta o no.
       {
         if(clave[0]==3&&clave[1]==8&&clave[2]==9&&clave[3]==2) // Si Ingresa clave para reset general del sistema.
            {write_eeprom(0,0);delay_ms(20);write_eeprom(1,0);delay_ms(20);// Reestablece a contraseña de Fabrica y reinicia Programa.
             write_eeprom(2,0);delay_ms(20);write_eeprom(3,0);delay_ms(20);
             reset_cpu();}
            
         if(clave[0]==contrasena[0]&&clave[1]==contrasena[1]&&clave[2]==contrasena[2]&&clave[3]==contrasena[3]) // Si las claves coinciden pasa a Menu Principal.
            {lcd_gotoxy(1,1);
            printf(lcd_putc,"                   ");
            lcd_gotoxy(1,2);
            printf(lcd_putc,"     Contraseña    ");
            lcd_gotoxy(1,3);
            printf(lcd_putc,"      Correcta     ");
            lcd_gotoxy(1,4);
            printf(lcd_putc,"                   ");
            delay_ms(500);Menu=50;unidad=11;printf(lcd_putc,"\f");Damper_on;flag_latencia=0;Latencia=0;tiempo_purga=1;
            t_latencia=0;flag_iniciodamper=1;}
         else                                         // Si la clave no coincide vuelve a mostrar el menu para ingresar la clave.
         {lcd_gotoxy(1,1);
            printf(lcd_putc,"");
            lcd_gotoxy(1,2);
            printf(lcd_putc,"     Contraseña    ");
            lcd_gotoxy(1,3);
            printf(lcd_putc,"     Incorrecta    ");
            lcd_gotoxy(1,4);
            printf(lcd_putc,"                   ");
            delay_ms(500);unidad=11;printf(lcd_putc,"\f");}
       }
   
       if(unidad>11&&unidad<14)
          { lcd_gotoxy(unidad-1,4);// Para mostrar cursor.
            lcd_putc(t[1]);
          }

   }
//----------------Fin-Menu0---------------------------------------------------------------   

//------------Menu1------------------------------------------------------------------   
   if(Menu == 1){ // Menu de seleccion de lo que desea encender
      if(paso<0)
        {paso=0;}
   
   lcd_gotoxy(1,1);
   printf(lcd_putc,"---MENU PRINCIPAL---");
   
      if(UP)//Si oprime hacia arriba
      {  if(Flanco == 0) 
            {Flecha2--;Flecha--;Flecha1=Flecha+1;Flanco = 1;delay_ms(30);}}
         else
            {Flanco = 0;}
            
      if(DOWN)//Si oprime hacia abajo
      {  if(Flanco2 == 0) 
            {Flecha2++;Flecha++;Flecha1=Flecha-1;Flanco2 = 1;delay_ms(30);}}
         else
            {Flanco2 = 0;}
   
        
        if(Flecha2>nMenuH)
        {paso++;nMenuH=Flecha2;nMenuL=nMenuH-2;Flecha=4;}
        
        if(Flecha2<nMenuL)
        {paso--;nMenuL=Flecha2;nMenuH=nMenuL+2;Flecha=2;}
        
        if(Flecha2>n_opcionH)
        {Flecha2=n_opcionL;Flecha=2;paso=0;nMenuL=Flecha2;nMenuH=nMenuL+2;}
        
        if(Flecha2<n_opcionL)
        {Flecha2=n_opcionH;Flecha=4;paso=n_opcionH-4;nMenuH=Flecha2;nMenuL=nMenuH-2;}  
        
        mensajes(1+paso,2);
        mensajes(2+paso,3);
        mensajes(3+paso,4);

        lcd_gotoxy(1,Flecha);// Para mostrar la flecha de seleccion
        lcd_putc(t[0]);

        if(Flecha==2)
          {lcd_gotoxy(1,4);// Para mostrar la flecha de seleccion
            lcd_putc(t[1]);
            lcd_gotoxy(1,3);// Para mostrar la flecha de seleccion
            lcd_putc(t[1]);
          }
        
        if(Flecha==4)
          {lcd_gotoxy(1,2);// Para mostrar la flecha de seleccion
            lcd_putc(t[1]);
            lcd_gotoxy(1,3);// Para mostrar la flecha de seleccion
            lcd_putc(t[1]);
          }
        
        if(Flecha==3)
          { lcd_gotoxy(1,4);// Para mostrar la flecha de seleccion
            lcd_putc(t[1]);
            lcd_gotoxy(1,2);// Para mostrar la flecha de seleccion
            lcd_putc(t[1]);
          }

        if(RIGHT)// Si oprime derecha
      {  if(Flanco1 == 0) 
            {/*Flanco1 = 1;*/Menu=Flecha2;Flecha=3;delay_ms(500);printf(lcd_putc,"\f");
            if(Menu==6)
            {clave[0]=0;clave[1]=0;clave[2]=0;clave[3]=0;}}}
         else
            {Flanco1 = 0;}  
            
      if(LEFT)// Si oprime Izquierda
      {  if(Flanco3 == 0) 
            {/*Flanco3 = 1;*/delay_ms(500);Menu=20;printf(lcd_putc,"\f");}}
         else
            {Flanco3 = 0;}               

   }
//----------------Fin-Menu1---------------------------------------------------------------      

//----------------Menu2---------------------------------------------------------------
   if(Menu == 2){ // Menu de seleccion de estado de Alarma
   estadoalarma=!estadoalarma;Flanco1 = 1;Menu=1; paso=0;
                      
            if(estadoalarma==1)
            {estadoalarma=1;
               lcd_gotoxy(1,1);
               printf(lcd_putc,"                    ");
               lcd_gotoxy(1,2);
               printf(lcd_putc,"        Activo      ");
               lcd_gotoxy(1,3);
               printf(lcd_putc,"        Alarma      ");
               lcd_gotoxy(1,4);
               printf(lcd_putc,"                    ");}
               
            if(estadoalarma==0)
            {estadoalarma=0;
               lcd_gotoxy(1,1);
               printf(lcd_putc,"                    ");
               lcd_gotoxy(1,2);
               printf(lcd_putc,"      Desactivo     ");
               lcd_gotoxy(1,3);
               printf(lcd_putc,"       Alarma       ");
               lcd_gotoxy(1,4);
               printf(lcd_putc,"                    ");}
             
            write_eeprom(5,estadoalarma);delay_ms(1000);Flecha=2;Flecha2=2;printf(lcd_putc,"\f");
   }      
//----------------Fin-Menu2---------------------------------------------------------------

//----------------Menu3---------------------------------------------------------------
   if(Menu == 3){ // Menu de seleccion de SetPoint
   lcd_gotoxy(1,1);
   printf(lcd_putc,"  Duracion Actual   ");                          
   lcd_gotoxy(1,2);                                                  
   printf(lcd_putc," Tiempo= %02u:%02u:%02u ",horas,minutos,segundos);      
   
   if(flag_filtro==1)
   {
      lcd_gotoxy(1,3);                                                 
      printf(lcd_putc,"DESACTIVAR= Oprima >");                        
   }
   else
   {
      lcd_gotoxy(1,3);                                                 
      printf(lcd_putc," ACTIVAR= Oprima >  ");                        
   }
   
   lcd_gotoxy(1,4);                                                 
   printf(lcd_putc," RESET= Oprima ^    ");                        
                                                                    
   
      if(UP)//Si oprime hacia arriba
      {  delay_ms(200);
         printf(lcd_putc,"\f");
         lcd_gotoxy(1,2);
         printf(lcd_putc," Reset de tiempo ");
         lcd_gotoxy(1,3);
         printf(lcd_putc,"     Exitoso     ");
         write_eeprom(70,0);write_eeprom(71,0);write_eeprom(72,0);
         delay_ms(700);
         segundos=0;minutos=0;horas=0;
         delay_ms(30);Menu=1; paso=0;Flecha=2;Flecha2=2;printf(lcd_putc,"\f");
      }
      
      if(RIGHT)
      {
         flag_filtro=!flag_filtro;write_eeprom(73,flag_filtro);
         delay_ms(500);Menu=1; paso=0;Flecha=2;Flecha2=2;printf(lcd_putc,"\f");
      }
            
       if(LEFT)// Si oprime Izquierda
      {  if(Flanco3 == 0) 
            {Flanco3 = 1;delay_ms(500);Menu=1; paso=0;Flecha=2;Flecha2=2;printf(lcd_putc,"\f");}}
         else
            {Flanco3 = 0;}      
             
   }      
//----------------Fin-Menu3---------------------------------------------------------------

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

//----------------Menu5---------------------------------------------------------------
   if(Menu == 5){ // Menu para Cambio de Contraseña
   lcd_gotoxy(1,1);
   printf(lcd_putc,"      Ingrese       ");
   lcd_gotoxy(1,2);
   printf(lcd_putc," Contraseña Actual  ");
   lcd_gotoxy(1,3);
   printf(lcd_putc,"    CLAVE=%i%i%i%i  ",clave[0],clave[1],clave[2],clave[3]);
   
      
   if(UP)//Si oprime hacia arriba
      {  if(Flanco == 0) 
            {clave[unidad-11]++;Flanco = 1;delay_ms(30);}}
         else
            {Flanco = 0;}
            
      if(DOWN)//Si oprime hacia abajo
      {  if(Flanco2 == 0) 
            {clave[unidad-11]--;Flanco2 = 1;delay_ms(30);}}
         else
            {Flanco2 = 0;}
   
      if(RIGHT)// Si oprime derecha
      {  if(Flanco1 == 0) 
            {Flanco1 = 1;unidad++;delay_ms(500);printf(lcd_putc,"\f");}}
         else
            {Flanco1 = 0;}   
            
       if(LEFT)// Si oprime Izquierda
      {  if(Flanco3 == 0) 
            {Flanco3 = 1;unidad--;delay_ms(500);printf(lcd_putc,"\f");}}
         else
            {Flanco3 = 0;}    
            
       if(clave[unidad-11]<0)// Si la unidad donde se encuentra ubicado el cursor es menor que 0 pasa a 9.     
       {clave[unidad-11]=9;}
       if(clave[unidad-11]>9)// Si la unidad donde se encuentra ubicado el cursor es mayor que 9 pasa a 0.     
       {clave[unidad-11]=0;}
       if(unidad<11)
       {Menu=1;paso=0;Flecha=2;Flecha2=2;unidad=11;}
       lcd_gotoxy(unidad,4);// Para mostrar la flecha de seleccion
       lcd_putc(t[2]);
       if(unidad>14)// Si ya ingreso la contraseña muestra si es correcta o no, dependiendo si ingreso la clave correctamente.
       {
         if(clave[0]==contrasena[0]&&clave[1]==contrasena[1]&&clave[2]==contrasena[2]&&clave[3]==contrasena[3])
            {lcd_gotoxy(1,1);
            printf(lcd_putc,"                    ");
            lcd_gotoxy(1,2);
            printf(lcd_putc,"     Contraseña     ");
            lcd_gotoxy(1,3);
            printf(lcd_putc,"      Correcta      ");
            lcd_gotoxy(1,4);
            printf(lcd_putc,"                    ");
            delay_ms(500);Menu=51;unidad=11;printf(lcd_putc,"\f");
            clave[0]=0;clave[1]=0;clave[2]=0;clave[3]=0;}
         else
         {lcd_gotoxy(1,1);
            printf(lcd_putc,"                    ");
            lcd_gotoxy(1,2);
            printf(lcd_putc,"     Contraseña     ");
            lcd_gotoxy(1,3);
            printf(lcd_putc,"     Incorrecta     ");
            lcd_gotoxy(1,4);
            printf(lcd_putc,"                    ");
            delay_ms(500);unidad=11;printf(lcd_putc,"\f");}
       }
               
          if(unidad>11&&unidad<14)
          { lcd_gotoxy(unidad-1,4);// Para mostrar cursor.
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
   
      
      if(UP)//Si oprime hacia arriba
      {  if(Flanco == 0) 
            {clave[unidad-11]++;Flanco = 1;delay_ms(30);}}
         else
            {Flanco = 0;}
            
      if(DOWN)//Si oprime hacia abajo
      {  if(Flanco2 == 0) 
            {clave[unidad-11]--;Flanco2 = 1;delay_ms(30);}}
         else
            {Flanco2 = 0;}
   
      if(RIGHT)// Si oprime Derecha
      {  if(Flanco1 == 0) 
            {Flanco1 = 1;unidad++;delay_ms(30);printf(lcd_putc,"\f");}}
         else
            {Flanco1 = 0;}   
            
       if(LEFT)// Si oprime Izquierda
      {  if(Flanco3 == 0) 
            {Flanco3 = 1;unidad--;delay_ms(30);printf(lcd_putc,"\f");}}
         else
            {Flanco3 = 0;}    
            
       if(clave[unidad-11]<0)// Si la unidad donde se encuentra ubicado el cursor es menor que 0 pasa a 9.     
       {clave[unidad-11]=9;}
       if(clave[unidad-11]>9)// Si la unidad donde se encuentra ubicado el cursor es mayor que 9 pasa a 0.     
       {clave[unidad-11]=0;}
       if(unidad<11)
       {Menu=5;unidad=11;}
       lcd_gotoxy(unidad,4);// Para mostrar la flecha de seleccion
       lcd_putc(t[2]);
       if(unidad>14)// Si ya ingreso la nueva contraseña.
       {
            lcd_gotoxy(1,1);
            printf(lcd_putc,"                    ");
            lcd_gotoxy(1,2);
            printf(lcd_putc,"     Contraseña     ");
            lcd_gotoxy(1,3);
            printf(lcd_putc,"     Almacenada     ");
            lcd_gotoxy(1,4);
            printf(lcd_putc,"                    ");
            write_eeprom(0,clave[0]);delay_ms(20);write_eeprom(1,clave[1]);delay_ms(20);
            write_eeprom(2,clave[2]);delay_ms(20);write_eeprom(3,clave[3]);delay_ms(20);
            delay_ms(500);Menu=1;paso=0;Flecha=2;Flecha2=2;
       }
      
           if(unidad>11&&unidad<14)
          { lcd_gotoxy(unidad-1,4);// Para mostrar cursor.
            lcd_putc(t[1]);
          }
   }      
//----------------Fin-Menu5.1---------------------------------------------------------------

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
      
      if(UP)
      {
         if(unidad2==1)
         {
            purga_minutos_pro++;
            if(purga_minutos_pro>59)
               purga_minutos_pro=0;
               
            delay_ms(300);
            
         }
         
         if(unidad2==3)
         {
            purga_segundos_pro++;
            if(purga_segundos_pro>59)
               purga_segundos_pro=0;
               
            delay_ms(300);
         }
      }
      
      if(DOWN)
      {
         if(unidad2==1)
         {
            purga_minutos_pro--;
            if(purga_minutos_pro<0)
               purga_minutos_pro=59;
               
            delay_ms(300);
         }
         
         if(unidad2==3)
         {
            purga_segundos_pro--;
            if(purga_segundos_pro<0)
               purga_segundos_pro=59;
               
            delay_ms(300);
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
            else
            {
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
         write_eeprom(20,purga_minutos_pro);write_eeprom(21,purga_segundos_pro);
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
            ppurga_minutos_pro++;
            if(ppurga_minutos_pro>59)
               ppurga_minutos_pro=0;
               
            delay_ms(300);
            
         }
         
         if(unidad2==3)
         {
            ppurga_segundos_pro++;
            if(ppurga_segundos_pro>59)
               ppurga_segundos_pro=0;
               
            delay_ms(300);
         }
      }
      
      if(DOWN)
      {
         if(unidad2==1)
         {
            ppurga_minutos_pro--;
            if(ppurga_minutos_pro<0)
               ppurga_minutos_pro=59;
               
            delay_ms(300);
         }
         
         if(unidad2==3)
         {
            ppurga_segundos_pro--;
            if(ppurga_segundos_pro<0)
               ppurga_segundos_pro=59;
               
            delay_ms(300);
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
         write_eeprom(22,ppurga_minutos_pro);write_eeprom(23,ppurga_segundos_pro);
         ppurga_minutos=ppurga_minutos_pro;ppurga_segundos=ppurga_segundos_pro;
         delay_ms(700);
         delay_ms(30);Menu=1; paso=0;Flecha=2;Flecha2=2;printf(lcd_putc,"\f");
      }  
       
   }
//----------------Fin-Menu7---------------------------------------------------------------

//-------------------Menu20----------------------------------------------------------------  

   if(Menu==20)
   {
      if(Lectura2==1)
      {
      lcd_gotoxy(1,1);                     
      printf(lcd_putc," JP CEGH");//T= %2.0f",sensores(1));//T= %2.1f",TC);
      lcd_gotoxy(1,2);
      //printf(lcd_putc,"Ext= %03u%%",FeedbackDamper);
      printf(lcd_putc,"Ext= ---");
      lcd_gotoxy(1,3);
      //printf(lcd_putc,"InFlow= %2.1f (m/s) ",Flujo);
      printf(lcd_putc,"InFlow= ---   ");
      lcd_gotoxy(11,2);
      printf(lcd_putc,"Flujo=--- ");  

      Lectura2=0;
      }
      
      if(flag_filtro==1)
      {
         if(horas>=setpoint)
         {
            lcd_gotoxy(10,1);                     
            printf(lcd_putc,"Filtro:CAM");
         }
         else
         {
            lcd_gotoxy(10,1);                     
            printf(lcd_putc,"Filtro:OK ");
         }
      }
      else
      {
         lcd_gotoxy(10,1);                     
         printf(lcd_putc,"Filtro:---");
      }
         
           
      if(estadofl==1)
      {
         Luz_Blanca_on;
         lcd_gotoxy(1,4);
         printf(lcd_putc,"Luz:ON ");
      }
      else
      {
         Luz_Blanca_off;
         lcd_gotoxy(1,4);
         printf(lcd_putc,"Luz:OFF ");
      }
      
      if(estadomv==1)
      {
         lcd_gotoxy(10,4);
         printf(lcd_putc,"Motor:ON ");
      }
      else
      {
         if(iniciodamper==1)
         {
            Motor_off;     
         }
         lcd_gotoxy(10,4);
         printf(lcd_putc,"Motor:OFF "); 
      }
      
      if(DOWN)//Si oprime hacia Abajo
      {  if(Flanco == 0) 
            {Flanco = 1;delay_ms(200);estadofl=!estadofl;}}
         else
            {Flanco = 0;}
            
      if(LEFT)//Si oprime hacia Izquierda
      {  
         if(Flanco2 == 0)
         {
            Flanco2 = 1;delay_ms(200);estadomv=!estadomv;
            if(estadomv==1)
            {
               Motor_on;
               delay_ms(150);
               lcd_init();
            }
            else
            {
               Motor_off;
               delay_ms(150);
               lcd_init();
            }
         }
      }
      else
      {
         Flanco2 = 0;
      }
   
      if(UP)// Si oprime Arriba
      {  if(Flanco1 == 0) 
            {/*Flanco1 = 1;*/delay_ms(500);printf(lcd_putc,"\f");Menu=1;paso=0;Flecha=2;Flecha2=2;unidad=11;}}
         else
            {Flanco1 = 0;}   
            
       if(RIGHT)// Si oprime Derecha
      {  if(Flanco3 == 0) 
            {delay_ms(500);printf(lcd_putc,"\f");Menu=40;}}
         else
            {Flanco3 = 0;}    
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
            write_eeprom(70,segundos);write_eeprom(71,minutos);write_eeprom(72,horas);
            printf(lcd_putc,"\f");
            lcd_gotoxy(1,2);
            printf(lcd_putc,"     Proceso      ");
            lcd_gotoxy(1,3);
            printf(lcd_putc,"    Finalizado    ");
            delay_ms(2000);
            printf(lcd_putc,"\f");
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
  
   
      if(RIGHT)// Si oprime derecha
      {  if(Flanco1 == 0) 
            {Flanco1 = 1;Menu=30;intensidadpantalla(contraste);Alarma_off;printf(lcd_putc,"\f");tiempo_ppurga=1;
            }}
         else
            {Flanco1 = 0;}   
            
       if(LEFT)// Si oprime Izquierda
      {  if(Flanco3 == 0) 
            {Flanco3 = 1;Menu=20;paso=0;Flecha=2;Flecha2=2;unidad=11;delay_ms(500);printf(lcd_putc,"\f");}}
         else
            {Flanco3 = 0;}    
             
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
      
   Motor_on;
   Luz_Blanca_off;
      
   if(LEFT)
   {
      tiempo_purga=0;Motor_on;Luz_Blanca_on;estadomv=1;estadofl=1;Menu=20;delay_ms(1000);
   }
      
   }      
//----------------Fin-Menu50---------------------------------------------------------------  

   if(tiemporeset>=20)
   {
      lcd_init();
      tiemporeset=0;
   } 

   if(ModoSleep==1)
      {intensidadpantalla(0);sleep();}

   if(Menu!=0 || Menu!=30)
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
      }
      Lectura=0;
   }   
   else
   {
      intensidadpantalla(contraste);Alarma_off;
   }

   }       
}

