// Cuatro pulsadores de entrada (Derecha, Izquierda, arriba y abajo).
// Reloj con XT de 4 MHz.
// Programaci?n para Cabina Extractora de Gases y humos.
// Tiene Contrase?a de incio para permitir funcionamiento de Cabina.
// Tiene Men?:Luz Blanca, Motor Ventilador, Intensidad de Pantalla, Alarma y Cambio de Contrase?a.
// Tiene Lector de velocidad de aire.
// Tiene control de velocidad de aire por Damper.
// Ing. Juan David Pi?eros.
// JP Inglobal.
// Lectura de sensor de vidrio.

// Esta version tiene la opcion de cargar los valores por defecto, cusndo el micro se pone a funcionar, despues de haber sido grabado
// Esto evita que los valores se reinicien con 255 en la EE

#include <18F4550.h>
#device adc=10

//#fuses XT,NOWDT,NOPROTECT,NOLVP,NODEBUG,VREGEN,NOMCLR// PLL1 para 4 MHz
//#use delay(clock=4000000)

#fuses HSPLL,NOWDT,NOPROTECT,NOLVP,NODEBUG,USBDIV,PLL1,CPUDIV1,NOVREGEN,MCLR// PLL1 para 4 MHz
#use delay(clock=48000000)

#include <math.h>

// Definici?n de teclado - NC
#define   UP            input(PIN_A5) // A5
#define   DOWN          input(PIN_E0) // E0
#define   RIGHT         input(PIN_E1) // E1
#define   LEFT          input(PIN_E2) // E2
#define   VIDRIOUP      !input(PIN_A4)
#define   VIDRIODN      !input(PIN_A3)

// Definici?n de otros puertos

#define   Luz_Blanca_on     output_bit(PIN_B4,1)
#define   Luz_Blanca_off    output_bit(PIN_B4,0)
#define   Motor_on          output_bit(PIN_B5,1)
#define   Motor_off         output_bit(PIN_B5,0)
#define   Alarma_on         output_bit(PIN_B6,1)
#define   Alarma_off        output_bit(PIN_B6,0)
#define   Rele_on           output_bit(PIN_B7,1)
#define   Rele_off          output_bit(PIN_B7,0)
#define   Pantalla_on       output_bit(PIN_C1,1)
#define   Pantalla_off      output_bit(PIN_C1,0)
#define   O1_on             output_bit(PIN_C6,1)
#define   O1_off            output_bit(PIN_C6,0)
#define   O2_on             output_bit(PIN_C7,1)
#define   O2_off            output_bit(PIN_C7,0)

signed int8 Menu=100, n_opcionH=7,n_opcionL=2,unidad=11,Flecha=2,Flecha1=3,Flecha2=2, nMenuL=2, nMenuH=4,contraste=10,unidad2=1,purga_minutos=0,purga_minutos_pro=0;
signed int8 Entero=0;
signed int8 segundos=0,minutos=0,horas=0,tiemporeset=0,purga_segundos=0,purga_segundos_pro=0,ppurga_segundos=0,ppurga_segundos_pro=0,ppurga_minutos=0,ppurga_minutos_pro=0;
short estadofl=0,estadomv=0,estadoalarma=0,Flanco=0,Flanco1=0,Flanco2=0,Flanco3=0,flag_latencia=0,Latencia=0,flag_filtro=0,estadopantalla=0;
short pulsu=0,pulsr=0,pulsd=0,pulsl=0;
short flag_min=0,flag_hrs=0,lee_cursor=0;
signed int8   paso=0;
char t[3]={'>',' ','^'};
char tecla='N';
signed  int8 clave[4]={0,0,0,0};   // 0=Verdadero, 1=Falso
int8 contrasena[4]={0,0,0,0};   // 0=Verdadero, 1=Falso
int8 Cambio=0;
float setpoint=0.0;
short ModoSleep,inicio=0,Lectura=0,flag_iniciodamper=0,tiempo_purga=0,tiempo_ppurga=0,ver_ddram=0;
int16 tiempos=0,t_latencia=0;
unsigned int8 ult_ddram=0;

#include <LCD420.c>

// Funcion para conversor analogo-digital
float sensores(int x){
float y;set_adc_channel(x);delay_ms(10);y=read_adc();return (y);
}

// Esta rutina hay que depurarla, pero sera despues, cuando lo haga la implementamos para ver si se escribio
// Algo raro en la pantalla por ruido y asi refrescar

/* void shw_vcursor()
{
 posic=get_cursor();   // get_cursor() esta en LCD420.c
 lcd_gotoxy(18,1);
 printf(lcd_putc,"%3u",posic); //Imprime en hexadecimal
 delay_ms(1000);
 // printf(lcd_putc,"\f");
 
} */

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

// ************************** Rutina unica para leer las teclas *********************

void lee_pulsad()
 {
   pulsu=0;pulsr=0;pulsd=0;pulsl=0;tecla='N';
   
   if(UP)//Si oprime hacia arriba
    {
      delay_ms(20); //Este retardo descarta que haya sido un pico espureo, si es asi no se hace nada
      if(UP)
      {
       if(Flanco == 0)
        {
         pulsu=1;  //Bandera que indica que la tecla UP, fue pulsada validamente
         tecla='U';
         Flanco=1;   //Flag que indica el ultimo estado del pulsador 
         Cambio=1;   //Flag que indica que uno de los 4 pulsadores fue presionado validamente
         delay_ms(30); //Retardo de salida para la repeticion de las teclas y el antirebote
        }
      }
     }else{
         Flanco=0; //Se reasigna el estado del pulsador en 0
      }
     
    if(RIGHT)//Si oprime hacia arriba
    {
      delay_ms(20); //Este retardo descarta que haya sido un pico espureo, si es asi no se hace nada
      if(RIGHT)
      {
       if(Flanco1 == 0)
        {
         pulsr=1;  //Bandera que indica que la tecla RIGHT, fue pulsada validamente
         tecla='R';
         Flanco1=1;   //Flag que indica el ultimo estado del pulsador 
         Cambio=1;   //Flag que indica que uno de los 4 pulsadores fue presionado validamente
         delay_ms(30); //Retardo de salida para la repeticion de las teclas y el antirebote
        }
      }
     }else{
         Flanco1=0; //Se reasigna el estado del pulsador en 0
      }
     
     if(DOWN)//Si oprime hacia arriba
    {
      delay_ms(20); //Este retardo descarta que haya sido un pico espureo, si es asi no se hace nada
      if(DOWN)
      {
       if(Flanco2 == 0)
        {
         pulsd=1;  //Bandera que indica que la tecla DOWN, fue pulsada validamente
         tecla='D';
         Flanco2=1;   //Flag que indica el ultimo estado del pulsador 
         Cambio=1;   //Flag que indica que uno de los 4 pulsadores fue presionado validamente
         delay_ms(30); //Retardo de salida para la repeticion de las teclas y el antirebote
        }
      }
     }else{
         Flanco2=0; //Se reasigna el estado del pulsador en 0
      }
     
    if(LEFT)//Si oprime hacia arriba
    {
      delay_ms(20); //Este retardo descarta que haya sido un pico espureo, si es asi no se hace nada
      if(LEFT)
      {
       if(Flanco3 == 0)
        {
         pulsl=1;  //Bandera que indica que la tecla LEFT, fue pulsada validamente
         tecla='L';
         Flanco3=1;   //Flag que indica el ultimo estado del pulsador 
         Cambio=1;   //Flag que indica que uno de los 4 pulsadores fue presionado validamente
         delay_ms(30); //Retardo de salida para la repeticion de las teclas y el antirebote
        }
      }
     }else{
         Flanco3=0; //Se reasigna el estado del pulsador en 0
      }
     
 }

#int_TIMER1
void temp1s(void){
   
   set_timer1(5536);  // 5  ms
   tiempos++;

   
   if(tiempos>=200)   // Ha transcurrido una decima de segundo (PIC18F4550 con XT = 16MHZ)
   {
      tiempos=0;tiemporeset++;
      Lectura=1;
      ver_ddram=1;
      lee_cursor=1;
      
      if(flag_latencia){
         t_latencia++;
      }
         
      if(estadomv==1 && flag_filtro==1)
      {  
         segundos++;
         if(segundos>=60)
         {
            segundos=0;minutos++;flag_min=1;
         }
         
         if(minutos>=60)
         {
            minutos=0;horas++;flag_hrs=1;
         }
      }
      
      if(tiempo_purga==1)   // Aqui se decrementa el tiempo de purga, cargado de la eeprom
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
               tiempo_purga=0;Motor_on;Menu=20;Cambio=1; // Aqui se sale al menu20 despues de la purga
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
   {lcd_gotoxy(2,y);printf(lcd_putc,"Cambio Contrase?a  ");}
      
   if(x==5)
   {lcd_gotoxy(2,y);printf(lcd_putc,"Tiempo de Purga    ");}
   
   if(x==6)
   {lcd_gotoxy(2,y);printf(lcd_putc,"Tiempo Post-Purga  ");}
}

void set_default()
 {
   write_eeprom(0,0);delay_ms(20);   // Contrase?a 0 = 0
   write_eeprom(1,0);delay_ms(20);   // Contrase?a 1 = 0
   write_eeprom(2,0);delay_ms(20);   // Contrase?a 2 = 0
   write_eeprom(3,0);delay_ms(20);   // Contrase?a 3 = 0
   
   write_eeprom(5,1);delay_ms(20);   // Estado de alarma = 1
   
   write_eeprom(20,1);delay_ms(20);  // Minutos de Tiempo de Purga = 1
   write_eeprom(21,0);delay_ms(20);  // Segundos de Tiempo de Purga = 0
   write_eeprom(22,1);delay_ms(20);  // Minutos de Tiempo de PostPurga = 1
   write_eeprom(23,0);delay_ms(20);  // Segundos de Tiempo de PostPurga = 0
   
   write_eeprom(40,60);delay_ms(20); // Setpoint de vida del filtro = 60
   
   write_eeprom(70,0);delay_ms(20);  // Segundos de uso del filtro = 0
   write_eeprom(71,0);delay_ms(20);  // Minutos de uso del filtro = 0
   write_eeprom(72,0);delay_ms(20);  // Horas de uso del filtro = 0
   write_eeprom(73,1);delay_ms(20);  // Flag de uso del filtro = 1

   write_eeprom(80,0x27);delay_ms(20); //Testigo de quer ya se grabaron los valores = 0x27
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
   
   if(read_eeprom(80)!=0x27)
   set_default();
   
      
   // Lee la contrase?a almacenada en la eeprom para poder comprobar con la que escribe el usuario.
   contrasena[0]=read_eeprom(0);delay_ms(20); contrasena[1]=read_eeprom(1);delay_ms(20);
   contrasena[2]=read_eeprom(2);delay_ms(20); contrasena[3]=read_eeprom(3);delay_ms(20);
   estadoalarma=read_eeprom(5);
   delay_ms(20);

   purga_minutos=read_eeprom(20);
   delay_ms(20);
   purga_segundos=read_eeprom(21);
   delay_ms(20);
   purga_minutos_pro=purga_minutos;purga_segundos_pro=purga_segundos;
   ppurga_minutos=read_eeprom(22);
   delay_ms(20);
   ppurga_segundos=read_eeprom(23);
   ppurga_minutos_pro=ppurga_minutos;ppurga_segundos_pro=ppurga_segundos;
         
   setpoint=read_eeprom(40);                                                                                                                                                                                                                                               
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
   
   if(ver_ddram==1)                // Inicio de funcion que verifica si entraron ruidos a la pantalla ....
       {
        ver_ddram=0;
        if(lee_ddram()!=ult_ddram)
         {
          lcd_init();
          cambio=1;
         }
       } 
       
   while(Menu==200)
   {                        // Fin de funcion que verifica si entraron ruidos a la pantalla ....   
      if(inicio==0)
      {
      delay_ms(500);
        if(RIGHT)
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
            inicio=1;               // Condicion Inicial de (inicio)
            Menu=0;                 // Condicion Inicial de Menu
            ModoSleep=0;
            delay_ms(500);
       }
      }
      Cambio=1;
 
   }

//------------Menu0------------------------------------------------------------------   
   while(Menu == 0){ // Menu de Contrase?a para Poder iniciar el equipo
   
   flag_latencia=1;set_pwm1_duty(0);
      if(t_latencia>15)  // Define que se acabo el tiempo de espera y borra la pantalla y hace un reset por firmware
         {
            printf(lcd_putc,"\f");reset_cpu();
         }
   
  
   if(Cambio == 1)
   {
      lcd_gotoxy(1,1);
      printf(lcd_putc,"     Ingrese        ");
      lcd_gotoxy(1,2);
      printf(lcd_putc,"    Contrase?a      ");
      lcd_gotoxy(1,3);
      printf(lcd_putc,"    CLAVE=%i%i%i%i  ",clave[0],clave[1],clave[2],clave[3]);
      lcd_gotoxy(unidad,4); // Para mostrar cursor.
     lcd_putc(t[2]);
     /* printf(lcd_putc,"%3u",ult_ddram);
     delay_ms(10);
     printf(lcd_putc,"%3u",lee_ddram()); */
     Cambio=0;
    }
   lee_pulsad();
   
      Switch(tecla)
      {
   
       Case 'U':       //Si se oprimio el pulsador de subir
       {clave[unidad-11]++;t_latencia=0;}
       break;
       
       Case 'D':       //Si se oprimio el pulsador de bajar
       {clave[unidad-11]--;t_latencia=0;}
       break;
       
       Case 'R':       //Si se oprimio el pulsador de derecha
       {unidad++;delay_ms(30);printf(lcd_putc,"\f");t_latencia=0;}
       break;
       
       Case 'L':       //Si se oprimio el pulsador de izquierda
       {unidad--;delay_ms(30);printf(lcd_putc,"\f");t_latencia=0;}
       break;
          
      }      
            
       if(Cambio==1)
       {
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
            {set_default();
             reset_cpu();
            }
            
         if(clave[0]==contrasena[0]&&clave[1]==contrasena[1]&&clave[2]==contrasena[2]&&clave[3]==contrasena[3]) // Si las claves coinciden pasa a Menu Principal.
            {lcd_gotoxy(1,1);
            printf(lcd_putc,"                   ");
            lcd_gotoxy(1,2);
            printf(lcd_putc,"     Contrase?a    ");
            lcd_gotoxy(1,3);
            printf(lcd_putc,"      Correcta     ");
            lcd_gotoxy(1,4);
            printf(lcd_putc,"                   ");
            delay_ms(200);
            Motor_on;
            delay_ms(100);
            lcd_init();
            delay_ms(100);
            Menu=50;Cambio=1;unidad=11;printf(lcd_putc,"\f");flag_latencia=0;Latencia=0;tiempo_purga=1;Lectura=1;
            t_latencia=0;flag_iniciodamper=1;}
         else                                         // Si la clave no coincide vuelve a mostrar el menu para ingresar la clave.
         {lcd_gotoxy(1,1);
            printf(lcd_putc,"");
            lcd_gotoxy(1,2);
            printf(lcd_putc,"     Contrase?a    ");
            lcd_gotoxy(1,3);
            printf(lcd_putc,"     Incorrecta    ");
            lcd_gotoxy(1,4);
            printf(lcd_putc,"                   "); 
            Cambio=1;                              //Se activa el flag de pedir clave nuevamente, porque fue errada
            delay_ms(500);unidad=11;printf(lcd_putc,"\f");}
       }
   
       if(unidad>11&&unidad<14)
          { lcd_gotoxy(unidad-1,4);// Para mostrar cursor.
            lcd_putc(t[1]);
            lcd_gotoxy(unidad,4);// Para mostrar cursor.
            lcd_putc(t[2]);
  
          }
       
      }
   }
//----------------Fin-Menu0---------------------------------------------------------------    

 
//------------Menu1------------------------------------------------------------------   
   while(Menu == 1)      // Menu de seleccion de lo que desea encender
   {
   lee_pulsad();
   
      if(tecla=='U')       //Si oprime hacia arriba
      {Flecha2--;Flecha--;Flecha1=Flecha+1;delay_ms(30);}
      if(tecla=='D')       //Si oprime hacia abajo
      {Flecha2++;Flecha++;Flecha1=Flecha-1;delay_ms(30);}
 
        
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
        
        if(Cambio==1)
        {
        lcd_gotoxy(1,1);
        printf(lcd_putc,"---MENU PRINCIPAL---");
        
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
          Cambio=0;
        }

        if(tecla=='R')        // Si oprime derecha
        {Menu=Flecha2;Cambio=1;Flecha=3;printf(lcd_putc,"\f");
            if(Menu==6)
            {clave[0]=0;clave[1]=0;clave[2]=0;clave[3]=0;}}
             
        if(tecla=='L')        // Si oprime Izquierda
        {Menu=20;Cambio=1;Cambio=1;printf(lcd_putc,"\f");}

   }
//----------------Fin-Menu1---------------------------------------------------------------      

//----------------Menu2---------------------------------------------------------------
   if(Menu == 2){ // Menu de seleccion de estado de Alarma
   estadoalarma=!estadoalarma;Menu=1; paso=0;
                      
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
             
            write_eeprom(5,estadoalarma);menu=1;Cambio=1;delay_ms(1000);Flecha=2;Flecha2=2;printf(lcd_putc,"\f");
   }      
//----------------Fin-Menu2---------------------------------------------------------------

//----------------Menu3---------------------------------------------------------------
   
   while(Menu == 3){ // Menu de seleccion de SetPoint
   if(Cambio==1)
   {
      lcd_gotoxy(1,1);
      Cambio=0;
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
   }
     
     lee_pulsad();
     
     Switch(tecla)
     
     {
   
      Case ('U'):       //Si oprime hacia arriba
      {  delay_ms(200);
         printf(lcd_putc,"\f");
         lcd_gotoxy(1,2);
         printf(lcd_putc," Reset de tiempo ");
         lcd_gotoxy(1,3);
         printf(lcd_putc,"     Exitoso     ");
         write_eeprom(70,0);
         delay_ms(20);
         write_eeprom(71,0);
         delay_ms(20);
         write_eeprom(72,0);
         delay_ms(700);
         segundos=0;minutos=0;horas=0;
         delay_ms(30);Menu=1; paso=0;Flecha=2;Flecha2=2;Cambio=1;printf(lcd_putc,"\f");
      }
      break;
      
      Case ('R'):       //Se oprimio el boton derecha
      {
         flag_filtro=!flag_filtro;write_eeprom(73,flag_filtro);
         if(flag_filtro==1) // Si se activa el filtro, se fuerza a leer el estado de una vez
         {
          flag_hrs=1;
         }
         delay_ms(500);Menu=1; paso=0;Flecha=2;Flecha2=2;Cambio=1;printf(lcd_putc,"\f");
      }
      break;
            
       Case ('L'):       // Si oprime Izquierda
      {  
        delay_ms(500);Menu=1; paso=0;Flecha=2;Flecha2=2;Cambio=1;printf(lcd_putc,"\f");
      }
      break;
             
   }
   }
//----------------Fin-Menu3---------------------------------------------------------------

//----------------Menu4---------------------------------------------------------------
   while(Menu == 4){ // Menu de seleccion de SetPoint
   if(Cambio==1)
   {
      lcd_gotoxy(1,2);
      printf(lcd_putc,"    Vida Filtro     ");
      lcd_gotoxy(1,3);
      printf(lcd_putc,"      %03.0f Horas  ",setpoint);
      Cambio=0;
   }
   
   if(setpoint<1.0)
   {setpoint=1.0;}
   
   if(setpoint>255.0)
   {setpoint=255.0;}
   
   lee_pulsad();
   
   Switch(tecla)
   {
   
    Case 'U':     //Si oprime hacia arriba
      {setpoint+=1.0;delay_ms(30);}
    break;
     
            
    Case 'D':     //Si oprime hacia abajo
      {setpoint-=1.0;delay_ms(30);}
    break;
        
   
    Case 'R':    //Si se oprime derecha
      {
        Menu=1;paso=0;Flecha=2;Flecha2=2;unidad=11;delay_ms(500);printf(lcd_putc,"\f");
        Entero=(int)setpoint;
        write_eeprom(40,Entero);
      }
    break;
         
    Case 'L':    // Si oprime Izquierda
      {Menu=1;paso=0;Flecha=2;Flecha2=2;unidad=11;delay_ms(500);printf(lcd_putc,"\f");}
       setpoint=read_eeprom(40);
    break;
         
      }
             
   }      
//----------------Fin-Menu4---------------------------------------------------------------

//----------------Menu5---------------------------------------------------------------
   while(Menu == 5)   // Menu para Cambio de Contrase?a
   {
      if(Cambio==1)
      {
         lcd_gotoxy(1,1);
         printf(lcd_putc,"      Ingrese       ");
         lcd_gotoxy(1,2);
         printf(lcd_putc," Contrase?a Actual  ");
         lcd_gotoxy(1,3);
         printf(lcd_putc,"    CLAVE=%i%i%i%i  ",clave[0],clave[1],clave[2],clave[3]);
         lcd_gotoxy(unidad,4); // Para mostrar cursor.
         lcd_putc(t[2]);
         Cambio=0;
      }
   
   lee_pulsad();
   Switch (tecla)
   {
   
   Case 'U':  //Si oprime hacia arriba
      {clave[unidad-11]++;delay_ms(30);}
      break;
        
   Case 'D': //Si oprime hacia abajo
      {clave[unidad-11]--;Flanco2 = 1;delay_ms(30);}
      break;
   
   Case 'R': // Si oprime derecha
      {unidad++;delay_ms(500);printf(lcd_putc,"\f");}
      break;
        
   Case 'L': // Si oprime Izquierda
      {unidad--;delay_ms(500);printf(lcd_putc,"\f");}
      break;
      
   }
   
   if(Cambio==1)
    {
        
       if(clave[unidad-11]<0)// Si la unidad donde se encuentra ubicado el cursor es menor que 0 pasa a 9.     
       {clave[unidad-11]=9;}
       if(clave[unidad-11]>9)// Si la unidad donde se encuentra ubicado el cursor es mayor que 9 pasa a 0.     
       {clave[unidad-11]=0;}
       if(unidad<11)
       {
        Menu=1;paso=1;Flecha=2;Flecha1=3;Flecha2=2;unidad=11;
        lcd_gotoxy(unidad,4);
        printf(lcd_putc,"\f");
        unidad=11;clave[0]=0;clave[1]=0;clave[2]=0;clave[3]=0;
       }
       else
       {
       lcd_gotoxy(unidad,4);// Para mostrar la flecha de seleccion
       lcd_putc(t[2]);
       }
       if(unidad>14)// Si ya ingreso la contrase?a muestra si es correcta o no, dependiendo si ingreso la clave correctamente.
       {
         if(clave[0]==contrasena[0]&&clave[1]==contrasena[1]&&clave[2]==contrasena[2]&&clave[3]==contrasena[3])
            {lcd_gotoxy(1,1);
            printf(lcd_putc,"                    ");
            lcd_gotoxy(1,2);
            printf(lcd_putc,"     Contrase?a     ");
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
            printf(lcd_putc,"     Contrase?a     ");
            lcd_gotoxy(1,3);
            printf(lcd_putc,"     Incorrecta     ");
            lcd_gotoxy(1,4);
            printf(lcd_putc,"                    ");
            delay_ms(500);unidad=11;printf(lcd_putc,"\f");
            clave[0]=0;clave[1]=0;clave[2]=0;clave[3]=0;
            }
         }
          if(unidad>11&&unidad<14)
          { lcd_gotoxy(unidad-1,4);// Para mostrar cursor.
            lcd_putc(t[1]);
            lcd_gotoxy(unidad,4);// Para mostrar cursor.
            lcd_putc(t[2]);
          }
    }
   }      
//----------------Fin-Menu5---------------------------------------------------------------

//----------------Menu5.1---------------------------------------------------------------
   while(Menu == 51)   // Menu cuando ingresa correctamente la contrase?a, permite que digite nueva contrase?a.
   {
    if(Cambio==1)
    {
   lcd_gotoxy(1,1);
   printf(lcd_putc,"     Ingrese        ");
   lcd_gotoxy(1,2);
   printf(lcd_putc,"  Contrase?a Nueva  ");
   lcd_gotoxy(1,3);
   printf(lcd_putc,"    CLAVE=%i%i%i%i  ",clave[0],clave[1],clave[2],clave[3]);
   lcd_gotoxy(unidad,4);// Para mostrar la flecha de seleccion
   lcd_putc(t[2]);
   Cambio=0;
    }
   
   lee_pulsad();
   Switch (tecla)
    {
    Case 'U':  //Si oprime hacia arriba
      {clave[unidad-11]++;delay_ms(30);}
      break;
      
    Case 'D':  //Si oprime hacia abajo
      {clave[unidad-11]--;delay_ms(30);}
      break;
    
    Case 'R':  // Si oprime Derecha
      {unidad++;delay_ms(30);printf(lcd_putc,"\f");}
      break;
    
    Case 'L':  // Si oprime Izquierda
      {unidad--;delay_ms(30);printf(lcd_putc,"\f");}
      break;
    }
    
    if(Cambio==1)
      {      
            
       if(clave[unidad-11]<0)// Si la unidad donde se encuentra ubicado el cursor es menor que 0 pasa a 9.     
       {clave[unidad-11]=9;}
       if(clave[unidad-11]>9)// Si la unidad donde se encuentra ubicado el cursor es mayor que 9 pasa a 0.     
       {clave[unidad-11]=0;}
       if(unidad<11)
       {
        Menu=1;paso=1;Flecha=2;Flecha1=3;Flecha2=2;unidad=11;
        lcd_gotoxy(unidad,4);
        printf(lcd_putc,"\f");
        unidad=11;clave[0]=0;clave[1]=0;clave[2]=0;clave[3]=0;
       }
       lcd_gotoxy(unidad,4);// Para mostrar la flecha de seleccion
       lcd_putc(t[2]);
       if(unidad>14)// Si ya ingreso la nueva contrase?a.
       {
            lcd_gotoxy(1,1);
            printf(lcd_putc,"                    ");
            lcd_gotoxy(1,2);
            printf(lcd_putc,"     Contrase?a     ");
            lcd_gotoxy(1,3);
            printf(lcd_putc,"     Almacenada     ");
            lcd_gotoxy(1,4);
            printf(lcd_putc,"                    ");
            write_eeprom(0,clave[0]);delay_ms(20);write_eeprom(1,clave[1]);delay_ms(20);
            write_eeprom(2,clave[2]);delay_ms(20);write_eeprom(3,clave[3]);delay_ms(20);
            contrasena[0]=clave[0];contrasena[1]=clave[1];contrasena[2]=clave[2];contrasena[3]=clave[3];
            clave[0]=0;clave[1]=0;clave[2]=0;clave[3]=0;
            delay_ms(500);Menu=1;Flecha=2;Flecha2=2;unidad=11;printf(lcd_putc,"\f");
       }
      
           if(unidad>11&&unidad<14)
          { lcd_gotoxy(unidad-1,4);// Para mostrar cursor.
            lcd_putc(t[1]);
          }
      }
   }      
//----------------Fin-Menu5.1---------------------------------------------------------------

//----------------Menu6---------------------------------------------------------------
   while(Menu == 6)// Menu de Tiempo de Purga
   {
    if(Cambio==1)
     {
      lcd_gotoxy(1,1);
      printf(lcd_putc,"     Tiempo  de      ");
      lcd_gotoxy(1,2);
      printf(lcd_putc,"       Purga         ");
      lcd_gotoxy(6,3);
      printf(lcd_putc,"%02u:%02u (M:S)  ",purga_minutos_pro,purga_segundos_pro); 
      
      lcd_gotoxy(unidad2+6,4);// Para mostrar cursor.
      lcd_putc(t[2]);
      Cambio=0;
     }
      
      if(unidad2<1)
         unidad2=1;
      if(unidad2>3)
         unidad2=3;   
     
     lee_pulsad();
     
     Switch(tecla)
       
      {
      Case 'U':
      {
         if(unidad2==1)
         {
            purga_minutos_pro++;
            if(purga_minutos_pro>59)
               purga_minutos_pro=0;
               
            delay_ms(200);
            
         }
         
         if(unidad2==3)
         {
            purga_segundos_pro++;
            if(purga_segundos_pro>59)
               purga_segundos_pro=0;
               
            delay_ms(200);
         }
      }
      break;
      
      Case 'D':
      {
         if(unidad2==1)
         {
            purga_minutos_pro--;
            if(purga_minutos_pro<0)
               purga_minutos_pro=59;
               
            delay_ms(200);
         }
         
         if(unidad2==3)
         {
            purga_segundos_pro--;
            if(purga_segundos_pro<0)
               purga_segundos_pro=59;
               
            delay_ms(200);
         }
      }
      break;
      
      Case 'R':
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
         delay_ms(400);
         printf(lcd_putc,"\f");
      }
      break;
      
      Case 'L':
      {
         delay_ms(200);
         printf(lcd_putc,"\f");
         lcd_gotoxy(1,2);
         printf(lcd_putc,"Tiempo Almacenado");
         lcd_gotoxy(1,3);
         printf(lcd_putc,"  Correctamente  ");
         write_eeprom(20,purga_minutos_pro);
         delay_ms(20);
         write_eeprom(21,purga_segundos_pro);
         delay_ms(20);
         clave[0]=0;clave[1]=0;clave[2]=0;clave[3]=0;
         purga_minutos=purga_minutos_pro;purga_segundos=purga_segundos_pro;
         delay_ms(700);
         Menu=1;Flecha=2;Flecha2=2;Cambio=1;paso=0;printf(lcd_putc,"\f");
      }
      break;
     }
        
   }
//----------------Fin-Menu6---------------------------------------------------------------

//----------------Menu7---------------------------------------------------------------
   while(Menu == 7)   // Menu de Tiempo de Post-Purga
   {
    if(Cambio==1)
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
      Cambio=0;
     }
     
     lee_pulsad();
     
     Switch(tecla)
      {
      
      Case 'U':
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
      break;
      
      Case 'D':
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
      break;
      
      Case 'R':
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
      break;
      
      Case 'L':
      {
         delay_ms(200);
         printf(lcd_putc,"\f");
         lcd_gotoxy(1,2);
         printf(lcd_putc,"Tiempo Almacenado");
         lcd_gotoxy(1,3);
         printf(lcd_putc,"  Correctamente  ");
         write_eeprom(22,ppurga_minutos_pro);
         delay_ms(20);
         write_eeprom(23,ppurga_segundos_pro);
         delay_ms(20);
         ppurga_minutos=ppurga_minutos_pro;ppurga_segundos=ppurga_segundos_pro;
         delay_ms(700);
         delay_ms(30);Menu=1;Cambio=1;Flecha=2;Flecha2=2;paso=0;printf(lcd_putc,"\f");
      }
      break;
      }
       
   }
//----------------Fin-Menu7---------------------------------------------------------------

//-------------------Menu20----------------------------------------------------------------  

   if(Menu==20)
   {
      if(Cambio==1)
      {
      lcd_send_byte(0,1); //Borra la pantalla, si se entra por primera vez
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
      lcd_gotoxy(10,1);                     
      printf(lcd_putc,"Filtro:OK ");
      
      if(estadofl==1)
      {
         Luz_Blanca_on;
         lcd_gotoxy(1,4);
         printf(lcd_putc,"Luz: ON ");
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
         printf(lcd_putc,"Motor: ON ");
      }
      else
      {   
         lcd_gotoxy(10,4);
         printf(lcd_putc,"Motor:OFF "); 
      }
      Cambio=0;
      }
  
      if(flag_filtro==1)
      {
        if(flag_hrs==1)
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
         flag_hrs=0;
        }  
      }
         
  // ********* Aqui comienza el monitoreo de teclas y tiempos         
     
     lee_pulsad();
     
      Switch (tecla)
      {
       Case 'D':       //Si oprime hacia Abajo
       { 
        estadofl=!estadofl;
       }
       break;
      
            
      Case 'L':      //Si oprime hacia Izquierda
      {  
         estadomv=!estadomv;
         if(estadomv){
            Motor_on;
         }else{
            Motor_off;
         }
         delay_ms(100);
         lcd_init();
         delay_ms(100);
         Cambio=1;
      }
      break;
      
   
       Case 'U':      // Si oprime Arriba
      {  
        printf(lcd_putc,"\f");Menu=1;paso=0;Flecha=2;Flecha2=2;unidad=11;
      }
      break;
            
       Case 'R':      // Si oprime Derecha
       {  
        printf(lcd_putc,"\f");Menu=40;
       }
       break;
   }
 }
//----------------Fin-Menu20---------------------------------------------------------------  

//-----------------Menu30------------------------------------------------------------------  
   while(Menu == 30)
   {
      if(Cambio==1)
      {
      lcd_gotoxy(1,1);
      printf(lcd_putc,"    !Post-Purga!    ");
      lcd_gotoxy(1,2);
      printf(lcd_putc,"                    ");
      Cambio=0;
      }
             
      if(Lectura==1)
      {
         Luz_Blanca_off;
         lcd_gotoxy(1,3);
         printf(lcd_putc,"      %02u:%02u  ",ppurga_minutos,ppurga_segundos);
         
         if(ppurga_segundos==0 && ppurga_minutos==0)
         {
            write_eeprom(70,segundos);
            delay_ms(20);
            write_eeprom(71,minutos);
            delay_ms(20);
            write_eeprom(72,horas);
            delay_ms(20);
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
   if(Menu == 40)   // Menu de apagado de cabina
   {
    if(Cambio==1)
     {
   lcd_gotoxy(1,1);
   printf(lcd_putc,"   Desea apagar     ");
   lcd_gotoxy(1,2);
   printf(lcd_putc,"     la cabina      ");
   lcd_gotoxy(1,3);
   printf(lcd_putc,"   > Aceptar        ");
   lcd_gotoxy(1,4);
   printf(lcd_putc,"   < Cancelar       ");
   Cambio=0;
      }
  
   lee_pulsad();
   
   Switch(tecla)
    {
      Case 'R': // Si oprime derecha
      {Menu=30;Cambio=1;intensidadpantalla(contraste);Alarma_off;printf(lcd_putc,"\f");tiempo_ppurga=1;}
      break;
           
            
      Case 'L': // Si oprime Izquierda
      {Menu=20;Cambio=1;paso=0;Flecha=2;Flecha2=2;unidad=11;delay_ms(500);printf(lcd_putc,"\f");}
    }       
   }      
//----------------Fin-Menu40---------------------------------------------------------------   

//----------------Menu50---------------------------------------------------------------
   While(Menu == 50) // Menu de Purga
   {
    if(Cambio==1)
    {
      lcd_gotoxy(1,1);
      printf(lcd_putc,"      !Purga!       ");
      lcd_gotoxy(1,2);
      printf(lcd_putc,"                    ");
      lcd_gotoxy(1,3);
      printf(lcd_putc,"      %02u:%02u   ",purga_minutos,purga_segundos);
      Cambio=0;
      Motor_on;
      Luz_Blanca_off;
    }
     
    if(Lectura==1)
     {
      lcd_gotoxy(1,3);
      printf(lcd_putc,"      %02u:%02u   ",purga_minutos,purga_segundos);
      Lectura=0;
     }
     
    if((LEFT) || ((purga_minutos==0) && (purga_segundos==0)))
    {
      delay_ms(100);
      if(LEFT || ((purga_minutos==0) && (purga_segundos==0))){
         tiempo_purga=0;Motor_on;Luz_Blanca_on;estadomv=1;estadofl=1;Menu=20;Cambio=1;delay_ms(500);
      }
    }
 
  }      
//----------------Fin-Menu50---------------------------------------------------------------  

  /* if(tiemporeset>=20)
   {
      lcd_init();
      tiemporeset=0;
   }*/ 

   if(ModoSleep==1)
      {intensidadpantalla(0);sleep();}

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
    }
}

