// Cuatro pulsadores de entrada (Derecha, Izquierda, arriba y abajo).
// Reloj con XT de 4 MHz.
// Programación para Cabina Extractora de Gases y humos.
// Tiene Contraseña de incio para permitir funcionamiento de Cabina.
// Tiene Menú:Luz Blanca, Motor Ventilador y Cambio de Contraseña.
// Ing. Juan David Piñeros.
// JP Inglobal.

#include <18F4550.h>
//#fuses HSPLL,NOWDT,NOPROTECT,NOLVP,NODEBUG,USBDIV,PLL1,CPUDIV1,NOVREGEN,NOMCLR// PLL1 para 4 MHz
//#fuses XT,NOWDT,NOPROTECT,NOLVP,NODEBUG,VREGEN,NOMCLR// PLL1 para 4 MHz
//#use delay(clock=4000000)
#fuses HSPLL,NOWDT,NOPROTECT,NOLVP,NODEBUG,USBDIV,PLL1,CPUDIV1,NOVREGEN,MCLR// PLL1 para 4 MHz
#use delay(clock=48000000)
#include <LCD420.c>

// Definición de teclado - NC
#define   UP            input(PIN_A0)
#define   DOWN          input(PIN_A1)
#define   RIGHT         input(PIN_A2)
#define   LEFT          input(PIN_A3)

// Definición de otros puertos
#define   Luz_UV_on         output_bit(PIN_D0,1)
#define   Luz_UV_off        output_bit(PIN_D0,0)
#define   Luz_Blanca_on     output_bit(PIN_D1,1)
#define   Luz_Blanca_off    output_bit(PIN_D1,0)
#define   Motor_L1_on       output_bit(PIN_D2,1)
#define   Motor_L1_off      output_bit(PIN_D2,0)
#define   Motor_L2_on       output_bit(PIN_D3,1)
#define   Motor_L2_off      output_bit(PIN_D3,0)
#define   Motor_L3_on       output_bit(PIN_D4,1)
#define   Motor_L3_off      output_bit(PIN_D4,0)
#define   Motor_L4_on       output_bit(PIN_D5,1)
#define   Motor_L4_off      output_bit(PIN_D5,0)

short estadouv=0,estadofl=0,estadomv=0,Flanco=0,Flanco1=0,Flanco2=0,Flanco3=0;
int8 Menu=0, n_opcionH=5,n_opcionL=2,unidad=11,Flecha=2,Flecha1=3,Flecha2=2, nMenuL=2, nMenuH=4,tiemporeset=0;
int8 contrasena[4]={0,0,0,0};   // 0=Verdadero, 1=Falso
signed int8   paso=0,Velocidad=1;
signed  int8 clave[4]={0,0,0,0};   // 0=Verdadero, 1=Falso
char t[3]={'>',' ','^'}; 
int16 tiempos=0;

#int_TIMER1
void temp1s(void){
   set_timer1(5536);  // 5 ms
   tiempos++;
   if(tiempos>=200)   // Ha transcurrido una decima de segundo (PIC18F4550 con XT = 16MHZ)
   {
      tiempos=0;tiemporeset++;
   }
}

void mensajes(int8 x,y)// Funcion para imprimir mensajes de Menu Principal.
{  
   if(x==1)
   {
      if(estadofl==0)
      {
         lcd_gotoxy(2,y);printf(lcd_putc,"LUZ BLANCA      OFF");
      }
      if(estadofl==1)
      {
         lcd_gotoxy(2,y);printf(lcd_putc,"LUZ BLANCA       ON");
      }
   }
   if(x==2)
   {
      if(estadomv==0)
      {
         lcd_gotoxy(2,y);printf(lcd_putc,"EXTRACTOR       OFF");
      }
      if(estadomv==1)
      {
         lcd_gotoxy(2,y);printf(lcd_putc,"EXTRACTOR        ON");
      }
   }
   if(x==3)
   {
      lcd_gotoxy(2,y);printf(lcd_putc,"CAMBIO CONTRASENA  ");
   }
}

void velocidades(int8 x)// Funcion para imprimir mensajes de Menu Principal.
{  
   if(x==0)
   {
      Motor_L2_off;Motor_L3_off;Motor_L4_off;Motor_L1_off;
   }
   if(x==1)
   {
      Motor_L2_off;Motor_L3_off;Motor_L4_off;Motor_L1_on;
   }
   if(x==2)
   {
      Motor_L3_off;Motor_L4_off;Motor_L1_off;Motor_L2_on;
   }
   if(x==3)
   {
      Motor_L4_off;Motor_L1_off;Motor_L2_off;Motor_L3_on;
   }
   if(x==4)
   {
      Motor_L2_off;Motor_L3_off;Motor_L1_off;Motor_L4_on;
   }
}

void main ()
{
   output_d(0);
   lcd_init();
   setup_timer_1(T1_INTERNAL|T1_DIV_BY_1);
   enable_interrupts(INT_TIMER1);
   enable_interrupts(global);
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
   
   //Solo para simulacion
   //write_eeprom(0,0);delay_ms(20);write_eeprom(1,0);delay_ms(20);
   //write_eeprom(2,0);delay_ms(20);write_eeprom(3,0);delay_ms(20);
            
   
   
   // Lee la contraseña almacenada en la eeprom para poder comprobar con la que escribe el usuario.
   contrasena[0]=read_eeprom(0);delay_ms(20); contrasena[1]=read_eeprom(1);delay_ms(20);
   contrasena[2]=read_eeprom(2);delay_ms(20); contrasena[3]=read_eeprom(3);delay_ms(20);
  
   while(true){

//------------Menu0------------------------------------------------------------------   
   if(Menu == 0){ // Menu de Contraseña para Poder iniciar el equipo
   lcd_gotoxy(1,1);
   printf(lcd_putc,"     Ingrese        ");
   lcd_gotoxy(1,2);
   printf(lcd_putc,"    Contraseña      ");
   lcd_gotoxy(1,3);
   printf(lcd_putc,"    CLAVE=%i%i%i%i  ",clave[0],clave[1],clave[2],clave[3]);
   
      
      if(UP)//Si oprime hacia arriba
      {
         if(Flanco == 0)
         {
            clave[unidad-11]++;Flanco = 1;delay_ms(30);
         }
      }
      else
      {
         Flanco = 0;
      }
            
      if(DOWN)//Si oprime hacia abajo
      {
         if(Flanco2 == 0)
         {
            clave[unidad-11]--;Flanco2 = 1;delay_ms(30);
         }
      }
      else
      {
         Flanco2 = 0;
      }
   
      if(RIGHT)// Si Oprime Derecha
      {
         if(Flanco1 == 0)
         {
            Flanco1 = 1;unidad++;delay_ms(30);printf(lcd_putc,"\f");
         }
      }
      else
      {
         Flanco1 = 0;
      }
            
      if(LEFT)// Si Oprime izquierda
      {
         if(Flanco3 == 0)
         {
            Flanco3 = 1;unidad--;delay_ms(30);printf(lcd_putc,"\f");
         }
      }
      else
      {
         Flanco3 = 0;
      }
            
      if(clave[unidad-11]<0)     // Si la unidad donde se encuentra ubicado el cursor es menor que 0 pasa a 9.
      {
         clave[unidad-11]=9;
      }
      if(clave[unidad-11]>9)     // Si la unidad donde se encuentra ubicado el cursor es mayor que 9 pasa a 0.
      {
         clave[unidad-11]=0;
      }
      if(unidad<11)             // Si trata de correr mas a la izquierda de la primera unidad, deja el cursor en esa posicion.
      {unidad=11;}
      lcd_gotoxy(unidad,4);// Para mostrar cursor.
      lcd_putc(t[2]);
      if(unidad>14)             // Si a Terminado de ingresar la clave, verifica si es correcta o no.
      {
         if(clave[0]==3&&clave[1]==8&&clave[2]==9&&clave[3]==2) // Si Ingresa clave para reset general del sistema.
         {
            write_eeprom(0,0);delay_ms(20);write_eeprom(1,0);delay_ms(20);// Reestablece a contraseña de Fabrica y reinicia Programa.
            write_eeprom(2,0);delay_ms(20);write_eeprom(3,0);delay_ms(20);
            reset_cpu();
         }
            
         if(clave[0]==contrasena[0]&&clave[1]==contrasena[1]&&clave[2]==contrasena[2]&&clave[3]==contrasena[3]) // Si las claves coinciden pasa a Menu Principal.
         {
            lcd_gotoxy(1,1);
            printf(lcd_putc,"                   ");
            lcd_gotoxy(1,2);
            printf(lcd_putc,"     Contraseña    ");
            lcd_gotoxy(1,3);
            printf(lcd_putc,"      Correcta     ");
            lcd_gotoxy(1,4);
            printf(lcd_putc,"                   ");
            delay_ms(500);Menu=1;unidad=11;printf(lcd_putc,"\f");
         }
         else                                         // Si la clave no coincide vuelve a mostrar el menu para ingresar la clave.
         {
            lcd_gotoxy(1,1);
            printf(lcd_putc,"");
            lcd_gotoxy(1,2);
            printf(lcd_putc,"     Contraseña    ");
            lcd_gotoxy(1,3);
            printf(lcd_putc,"     Incorrecta    ");
            lcd_gotoxy(1,4);
            printf(lcd_putc,"                   ");
            delay_ms(500);unidad=11;printf(lcd_putc,"\f");
         }
      }
   
      if(unidad>11&&unidad<14)
      {
         lcd_gotoxy(unidad-1,4);// Para mostrar cursor.
         lcd_putc(t[1]);
      }

   }
//----------------Fin-Menu0---------------------------------------------------------------   

//------------Menu1------------------------------------------------------------------   
   if(Menu == 1){ // Menu de seleccion de lo que desea encender
   lcd_gotoxy(1,1);
   printf(lcd_putc,"---MENU PRINCIPAL---");
      if(paso<0)
        {paso=0;}
      
      mensajes(1+paso,3);
      mensajes(2+paso,4);    
      
      if(LEFT)//Si oprime hacia arriba
      {  
         if(Flanco == 0)
         {
            Menu=3;Flanco = 1;delay_ms(30);
         }
      }
      else
      {
         Flanco = 0;
      }
            
      if(DOWN)//Si oprime hacia abajo
      {  
         if(Flanco2 == 0)
         {
            Flanco2 = 1;delay_ms(30);Menu=2;
         }
      }
      else
      {
         Flanco2 = 0;
      }    

      if(UP)// Si oprime derecha
      {
         if(Flanco1 == 0)
         {
            Menu=4;delay_ms(500);printf(lcd_putc,"\f");
            if(Menu==4)
            {clave[0]=0;clave[1]=0;clave[2]=0;clave[3]=0;}
         }
      }
      else
      {
         Flanco1 = 0;
      }

   }
//----------------Fin-Menu1---------------------------------------------------------------      
   
//----------------Menu2---------------------------------------------------------------
   if(Menu == 2){ // Menu de seleccion de Estado de Luz Blanca
      Menu=1; paso=0;estadofl=!estadofl;
            
            if(estadofl==1)
            {
               estadofl=1;Luz_Blanca_on;
               lcd_gotoxy(1,1);
               printf(lcd_putc,"                    ");
               lcd_gotoxy(1,2);
               printf(lcd_putc,"      Encendio      ");
               lcd_gotoxy(1,3);
               printf(lcd_putc,"     Luz Blanca     ");
               lcd_gotoxy(1,4);
               printf(lcd_putc,"                    ");
            }
           
            if(estadofl==0)
            {
               estadofl=0;Luz_Blanca_off;
               lcd_gotoxy(1,1);
               printf(lcd_putc,"                    ");
               lcd_gotoxy(1,2);
               printf(lcd_putc,"       Apago        ");
               lcd_gotoxy(1,3);
               printf(lcd_putc,"    Luz  Blanca     ");
               lcd_gotoxy(1,4);
               printf(lcd_putc,"                    ");
            }
             
            delay_ms(500);Flecha=2;Flecha2=2;printf(lcd_putc,"\f");   
   }      
//----------------Fin-Menu2---------------------------------------------------------------

//----------------Menu3---------------------------------------------------------------
   if(Menu == 3){ // Menu de seleccion de estado de Motor Ventilador
   estadomv=!estadomv;Flanco1 = 1;Menu=1; paso=0;
                      
            if(estadomv==1)
            {
               estadomv=1;velocidades(1);Luz_UV_on;
               lcd_gotoxy(1,1);
               printf(lcd_putc,"                    ");
               lcd_gotoxy(1,2);
               printf(lcd_putc,"      Encendio      ");
               lcd_gotoxy(1,3);
               printf(lcd_putc,"      Extractor     ");
               lcd_gotoxy(1,4);
               printf(lcd_putc,"                    ");
            }
               
            if(estadomv==0)
            {
               estadomv=0;velocidades(0);Luz_UV_off;
               lcd_gotoxy(1,1);
               printf(lcd_putc,"                    ");
               lcd_gotoxy(1,2);
               printf(lcd_putc,"       Apago        ");
               lcd_gotoxy(1,3);
               printf(lcd_putc,"     Extractor      ");
               lcd_gotoxy(1,4);
               printf(lcd_putc,"                    ");
            }
             
            delay_ms(500);Flecha=2;Flecha2=2;printf(lcd_putc,"\f");
   }      
//----------------Fin-Menu3---------------------------------------------------------------

//----------------Menu4---------------------------------------------------------------
   if(Menu == 4){ // Menu para Cambio de Contraseña
   lcd_gotoxy(1,1);
   printf(lcd_putc,"      Ingrese       ");
   lcd_gotoxy(1,2);
   printf(lcd_putc," Contraseña Actual  ");
   lcd_gotoxy(1,3);
   printf(lcd_putc,"    CLAVE=%i%i%i%i  ",clave[0],clave[1],clave[2],clave[3]);
   
      
      if(UP)//Si oprime hacia arriba
      {
         if(Flanco == 0)
         {
            clave[unidad-11]++;Flanco = 1;delay_ms(30);
         }
      }
      else
      {
         Flanco = 0;
      }
            
      if(DOWN)//Si oprime hacia abajo
      {  
         if(Flanco2 == 0)
         {
            clave[unidad-11]--;Flanco2 = 1;delay_ms(30);
         }
      }
      else
      {
         Flanco2 = 0;
      }
   
      if(RIGHT)// Si oprime derecha
      {  
         if(Flanco1 == 0)
         {
            Flanco1 = 1;unidad++;delay_ms(30);printf(lcd_putc,"\f");
         }
      }
      else
      {
         Flanco1 = 0;
      }
            
      if(LEFT)// Si oprime Izquierda
      {
         if(Flanco3 == 0)
         {
            Flanco3 = 1;unidad--;delay_ms(30);printf(lcd_putc,"\f");
         }
      }
      else
      {
         Flanco3 = 0;
      }
            
      if(clave[unidad-11]<0)// Si la unidad donde se encuentra ubicado el cursor es menor que 0 pasa a 9.     
      {
         clave[unidad-11]=9;
      }
      if(clave[unidad-11]>9)// Si la unidad donde se encuentra ubicado el cursor es mayor que 9 pasa a 0.     
      {
         clave[unidad-11]=0;
      }
      if(unidad<11)
      {
         Menu=1;paso=0;Flecha=2;Flecha2=2;unidad=11;printf(lcd_putc,"\f");delay_ms(500);
      }
      lcd_gotoxy(unidad,4);// Para mostrar la flecha de seleccion
      lcd_putc(t[2]);
      
      if(unidad>14)// Si ya ingreso la contraseña muestra si es correcta o no, dependiendo si ingreso la clave correctamente.
      {
         if(clave[0]==contrasena[0]&&clave[1]==contrasena[1]&&clave[2]==contrasena[2]&&clave[3]==contrasena[3])
         {
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
         }
         else
         {
            lcd_gotoxy(1,1);
            printf(lcd_putc,"                    ");
            lcd_gotoxy(1,2);
            printf(lcd_putc,"     Contraseña     ");
            lcd_gotoxy(1,3);
            printf(lcd_putc,"     Incorrecta     ");
            lcd_gotoxy(1,4);
            printf(lcd_putc,"                    ");
            delay_ms(500);unidad=11;printf(lcd_putc,"\f");
         }
      }
               
      if(unidad>11&&unidad<14)
      {
         lcd_gotoxy(unidad-1,4);// Para mostrar cursor.
         lcd_putc(t[1]);
      }
   }      
//----------------Fin-Menu4---------------------------------------------------------------

//----------------Menu5.1---------------------------------------------------------------
   if(Menu == 51){ // Menu cuando ingresa correctamente la contraseña, permite que digite nueva contraseña.
   lcd_gotoxy(1,1);
   printf(lcd_putc,"     Ingrese        ");
   lcd_gotoxy(1,2);
   printf(lcd_putc,"  Contraseña Nueva  ");
   lcd_gotoxy(1,3);
   printf(lcd_putc,"    CLAVE=%i%i%i%i  ",clave[0],clave[1],clave[2],clave[3]);
   
      
       if(UP)//Si oprime hacia arriba
       {
         if(Flanco == 0)
         {
            clave[unidad-11]++;Flanco = 1;delay_ms(30);
         }
       }
       else
       {
         Flanco = 0;
       }
            
       if(DOWN)//Si oprime hacia abajo
       {
         if(Flanco2 == 0)
         {
            clave[unidad-11]--;Flanco2 = 1;delay_ms(30);
         }
       }
       else
       {
         Flanco2 = 0;
       }
   
       if(RIGHT)// Si oprime Derecha
       {
         if(Flanco1 == 0)
         {
            Flanco1 = 1;unidad++;delay_ms(30);printf(lcd_putc,"\f");
         }
       }
       else
       {
         Flanco1 = 0;
       }
            
       if(LEFT)// Si oprime Izquierda
       {
         if(Flanco3 == 0)
         {
            Flanco3 = 1;unidad--;delay_ms(30);printf(lcd_putc,"\f");
         }
       }
       else
       {
         Flanco3 = 0;
       }
            
       if(clave[unidad-11]<0)// Si la unidad donde se encuentra ubicado el cursor es menor que 0 pasa a 9.     
       {
         clave[unidad-11]=9;
       }
       
       if(clave[unidad-11]>9)// Si la unidad donde se encuentra ubicado el cursor es mayor que 9 pasa a 0.
       {
         clave[unidad-11]=0;
       }
       
       if(unidad<11)
       {
         Menu=5;unidad=11;
       }
       
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
       {
         lcd_gotoxy(unidad-1,4);// Para mostrar cursor.
         lcd_putc(t[1]);
       }
   }      
//----------------Fin-Menu6.1---------------------------------------------------------------

   if(tiemporeset>=20)
   {
      lcd_init();
      tiemporeset=0;
   } 
   
   }
}

