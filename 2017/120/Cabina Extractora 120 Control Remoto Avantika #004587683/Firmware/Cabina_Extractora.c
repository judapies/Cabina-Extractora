// Pendiente ajustar tiempos con Timer 0
#include <18F4550.h>
#device adc=10
#fuses HS,NOWDT,NOPROTECT,NOLVP,NODEBUG,NOVREGEN,MCLR,NOPUT,NOCPD,NOSTVREN// PLL1 para 4 MHz
#use delay(clock=4000000)
#include <LCD420.c>
#include <math.h>

// Definición de teclado - NC
#define   RIGHT         input(PIN_A5)
#define   LEFT          input(PIN_E0)
#define   DOWN          input(PIN_E1)
#define   UP            input(PIN_E2)
#define   VIDRIOUP      !input(PIN_A4)
#define   VIDRIODN      !input(PIN_A3)

// Definición de otros puertos

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

signed int8 Menu=100, n_opcionH=12,n_opcionL=2,unidad=11,Flecha=2,Flecha1=3,Flecha2=2, nMenuL=2, nMenuH=4,contraste=10,unidad2=1,purga_minutos=0,purga_minutos_pro=0;
int8 Entero=0,Decimal1=0,Decimal2=0,Entero1=0,Decimal12=0,n=0,l=0,h=0,tmp=10,MediaMovil=0,negativo=10,Automa=10,Manual=10,Semi=10,ValorSemi=0;
signed int8 segundos=0,minutos=0,horas=0,tiemporeset=0,purga_segundos=0,purga_segundos_pro=0,ppurga_segundos=0,ppurga_segundos_pro=0,ppurga_minutos=0,ppurga_minutos_pro=0;
int8 FeedbackDamper=0;
short estadofl=0,estadomv=0,estadoalarma=0,estadopantalla=0,Flanco=0,Flanco1=0,Flanco2=0,Flanco3=0,flag_latencia=0,Latencia=0,flag_filtro=0;
signed int8   paso=0;
char t[3]={'>',' ','^'}; 
signed  int8 clave[4]={0,0,0,0};   // 0=Verdadero, 1=Falso
int8 contrasena[4]={0,0,0,0};   // 0=Verdadero, 1=Falso
float setpoint=0.0,zero_fabrica=203.0;
short ModoSleep,inicio=0,Lectura=0,Lectura2=0,flag_iniciodamper=0,iniciodamper=0,ModoManual=0,ModoAuto=0,ModoSemi=0,tiempo_purga=0,tiempo_ppurga=0;
int16 tiempos=0,t_latencia=0,tiempos2=0;
float Beta=3550,R25=1350,T0=298.15,TK=0,TC=0,Rdivisor=1000,Rntc=0.0,Voltaje0=0.0,Voltaje1=0.0;
 
float a=0.0000001418633343546420,b=-0.00009476749001431169,c= 0.040182822903506,d= -5.354852229527197;//Sensor 2 Posible bueno
float Flujo=0.0,Flujo2=0.0,x=0.0,x_uno=0.0,x_cuadrado=0.0,x_cubo=0.0,Temporal=0.0,Temporal2=0.0,Velocidad=0.0;
float Temporal1=0.0,Temporal12=0.0,Diferencia=0.0,Ajuste1=1.0;
float error=0.0;
float promedio[12]={0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
int1 BITS[32];
int16 Ta,Tb,TICK=0;
Int1 BIT_START, NUEVO_DATO;
int BYTE_IR[4];
int8 BIT_BYTE(int1 D7, int1 D6, int1 D5, int1 D4, int1 D3, int1 D2, int1 D1, int1 D0);
int8 Entro=0;

#int_EXT
void RB0(){
   Ta=get_timer1();
   TICK = Ta-tb;
   Tb = Ta;
   
   Entro=1;
   
   if(BIT_START==1){
       //BIT 1
       if(TICK>600 && TICK < 800)  // 700 con Pres 4 y 20MHz 
      {
         BITS[n]=1; n++;
      }
      //BIT 0
       if(TICK>200 && TICK < 500)  // 350 con Pres 4 y 20MHz 
      {
         BITS[n]=0; n++;
      }
      if(n>=32){
         NUEVO_DATO=1;
         BIT_START=0;
      }
   
   }
   //BIT STAR
   if(TICK>3000 && TICK < 4000 && BIT_START==0)  //3500 con Pres 4 y 20MHz 
   {
      BIT_START=1;
      n=0; 
   }
}

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

Float Leer_Sensor_Flujo(int media)
{
    Voltaje0=sensores(0)*5/1023;
    Voltaje1=sensores(1)*5/1023;
    Rntc=Rdivisor/((5.0/Voltaje1)-1);
    TK=1/((log(Rntc/R25)/Beta)+(1/T0));
    TC=TK-273.15;
    //correccion=Tcalibracion/sensores(1);// Se hace la division inversa debido a que se lee directamente el conversor y es una NTC.
    if(negativo==10)
    {
      x_uno=sensores(0)+Diferencia;
    }
    if(negativo==20)
    {
      x_uno=sensores(0)-Diferencia;
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

void controlar_flujo(void)
{
   if(Automa==20 && estadomv==1)
   {
      error=setpoint-Flujo;      // Cálculo del error ******
      if(error<0.2&&error>-0.2)
      {}
      else
      {
         if(error>0.0)
         {tmp+=1;}
               
         if(error<-0.0)
         {tmp-=1;}  
      }
      
      if(tmp>251)
      {tmp=251;}
      if(tmp<4)         
      {tmp=4;}      
      set_pwm1_duty (tmp);
   }
   
   if(Manual==20)
   {
      tmp=251;
      set_pwm1_duty (tmp);
   }
   
   if(Semi==20)
   {
      set_pwm1_duty (ValorSemi);
   }
}

#int_TIMER0
void temp0s(void){
   
   set_timer0(207);  // 5  ms
   tiempos++;
   tiempos2++;
   
   
   if(t_latencia>=3000)
   {Latencia=1;}
   
   if(flag_latencia==1)
   {
      t_latencia++;         
   }
   
   if(tiempos2>=20)
   {tiempos2=0;Lectura2=1;}
   
   if(tiempos>=200)   // Ha transcurrido una decima de segundo (PIC18F4550 con XT = 16MHZ)
   {
      tiempos=0;tiemporeset++;
      Lectura=1;
         
      if(estadomv==1 && flag_filtro==1)
      {  
         segundos++;
         if(segundos==60)
         {
            segundos=0;minutos++;
         }
         
         if(minutos==60)
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
               tiempo_purga=0;Motor_on;Menu=20;Rele_on;
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
   {lcd_gotoxy(2,y);printf(lcd_putc,"Setpoint           ");}
   
   if(x==4)
   {lcd_gotoxy(2,y);printf(lcd_putc,"Cambio Contraseńa  ");}
   
   if(x==5)
   {lcd_gotoxy(2,y);printf(lcd_putc,"Media Movil        ");}
   
   if(x==6)
   {lcd_gotoxy(2,y);printf(lcd_putc,"Punto Cero         ");}
   
   if(x==7)
   {lcd_gotoxy(2,y);printf(lcd_putc,"Mantenimiento      ");}
   
   if(x==8)
   {lcd_gotoxy(2,y);printf(lcd_putc,"Velocidad Motor    ");}
   
   if(x==9)
   {lcd_gotoxy(2,y);printf(lcd_putc,"Tiempo de Purga    ");}
   
   if(x==10)
   {lcd_gotoxy(2,y);printf(lcd_putc,"Tiempo Post-Purga  ");}
   
   if(x==11)
   {lcd_gotoxy(2,y);printf(lcd_putc,"Mover Vidrio       ");}
}

void ajustar_damper(int8 cicloutil){
   set_pwm1_duty (cicloutil);
}

void main ()
{
   setup_adc_ports(AN0_TO_AN2);
   setup_adc(ADC_CLOCK_DIV_32 );
   output_d(0); 
   setup_timer_0(RTCC_8_BIT|RTCC_DIV_128);
   setup_timer_1(T1_INTERNAL|T1_DIV_BY_4);
   EXT_INT_EDGE(0,H_TO_L); // 
   enable_interrupts(INT_EXT);
   enable_interrupts(INT_TIMER0);
   setup_timer_2(T2_DIV_BY_4, 255, 1); 
   setup_ccp1 (CCP_PWM);
   enable_interrupts(GLOBAL);
   set_pwm1_duty(0);
   intensidadpantalla(0);
   lcd_init();
   printf(lcd_putc,"\f");
            
   // Lee la contraseńa almacenada en la eeprom para poder comprobar con la que escribe el usuario.
   contrasena[0]=read_eeprom(0);delay_ms(20); contrasena[1]=read_eeprom(1);delay_ms(20);
   contrasena[2]=read_eeprom(2);delay_ms(20); contrasena[3]=read_eeprom(3);delay_ms(20);
   estadoalarma=read_eeprom(5);delay_ms(20);
   
   Manual=read_eeprom(10);
   delay_ms(20);
   Automa=read_eeprom(11);
   delay_ms(20);
   Semi=read_eeprom(12);
   delay_ms(20);
   ValorSemi=read_eeprom(13);
   delay_ms(20);
   tmp=ValorSemi;
   delay_ms(20);
   purga_minutos=read_eeprom(20);
   delay_ms(20);
   purga_segundos=read_eeprom(21);
   delay_ms(20);
   purga_minutos_pro=purga_minutos;
   delay_ms(20);
   purga_segundos_pro=purga_segundos;
   delay_ms(20);
   ppurga_minutos=read_eeprom(22);
   delay_ms(20);
   ppurga_segundos=read_eeprom(23);
   delay_ms(20);
   ppurga_minutos_pro=ppurga_minutos;
   delay_ms(20);
   ppurga_segundos_pro=ppurga_segundos;
   delay_ms(20);   
   Entero=read_eeprom(25);//Decimal1=read_eeprom(26);
   delay_ms(20);
   Temporal=read_eeprom(26);
   delay_ms(20);
   Temporal=Temporal/100.0;
   Ajuste1=Entero+Temporal;
   
   Entero=read_eeprom(40);//Decimal2=read_eeprom(41);
   delay_ms(20);
   Temporal=read_eeprom(41);
   delay_ms(20);
   Temporal=Temporal/100.0;
   setpoint=Entero+Temporal;
   
   Entero1=read_eeprom(60);//Decimal2=read_eeprom(41);
   delay_ms(20);
   Temporal1=read_eeprom(61);
   delay_ms(20);
   Temporal1=Temporal1/100.0;
   Diferencia=Entero1+Temporal1;
   
   negativo=read_eeprom(50);
   delay_ms(20);
   MediaMovil=read_eeprom(42);
   delay_ms(20);
   
   if(MediaMovil>12)
   {MediaMovil=12;}
   
   if(MediaMovil<1)
   {MediaMovil=1;}
   
   segundos=read_eeprom(70);
   delay_ms(20);
   minutos=read_eeprom(71);
   delay_ms(20);
   horas=read_eeprom(72);
   delay_ms(20);
   flag_filtro=read_eeprom(73);
   Menu=200;
  
   while(true){

      if(NUEVO_DATO==1){
         NUEVO_DATO=0;
         BYTE_IR[0]=BIT_BYTE(BITS[0],BITS[1],BITS[2],BITS[3],BITS[4],BITS[5],BITS[6],BITS[7]);
         BYTE_IR[1]=BIT_BYTE(BITS[8],BITS[9],BITS[10],BITS[11],BITS[12],BITS[13],BITS[14],BITS[15]);
         BYTE_IR[2]=BIT_BYTE(BITS[16],BITS[17],BITS[18],BITS[19],BITS[20],BITS[21],BITS[22],BITS[23]);
         BYTE_IR[3]=BIT_BYTE(BITS[24],BITS[25],BITS[26],BITS[27],BITS[28],BITS[29],BITS[30],BITS[31]);
      }

   if(BYTE_IR[3]==0XF2 && Menu!=200) //Bajar Vidrio
   {
      if(VIDRIOUP || !VIDRIODN)
      {
         O1_on;O2_off;
      }
      else
      {
         O1_off;O2_off;
      }
   }
   
   if(BYTE_IR[3]==0XE9 && Menu!=200) // Subir Vidrio
   {
      if(!VIDRIOUP || VIDRIODN)
      {
         O1_off;O2_on;
      }
      else
      {
         O1_off;O2_off;
      }
   }
  
   if(BYTE_IR[3]==0XE6)
   {
      O1_off;O2_off;
   }

   while(Menu==200)
   {  
      if(NUEVO_DATO==1){
         NUEVO_DATO=0;
         BYTE_IR[0]=BIT_BYTE(BITS[0],BITS[1],BITS[2],BITS[3],BITS[4],BITS[5],BITS[6],BITS[7]);
         BYTE_IR[1]=BIT_BYTE(BITS[8],BITS[9],BITS[10],BITS[11],BITS[12],BITS[13],BITS[14],BITS[15]);
         BYTE_IR[2]=BIT_BYTE(BITS[16],BITS[17],BITS[18],BITS[19],BITS[20],BITS[21],BITS[22],BITS[23]);
         BYTE_IR[3]=BIT_BYTE(BITS[24],BITS[25],BITS[26],BITS[27],BITS[28],BITS[29],BITS[30],BITS[31]);
      }
         intensidadpantalla(0);
         delay_ms(10);
         if(RIGHT || BYTE_IR[2]==0X1C)
         {
            delay_ms(500);
            if(RIGHT || BYTE_IR[2]==0X1C)
            {
               lcd_init();
               intensidadpantalla(contraste);
               delay_ms(200);
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
               BYTE_IR[2]=0X00;
            }
         }
   }

//------------Menu0------------------------------------------------------------------   
   if(Menu == 0){ // Menu de Contraseńa para Poder iniciar el equipo
      flag_latencia=1;set_pwm1_duty(0);
      if(Latencia==1)
      {
         printf(lcd_putc,"\f");reset_cpu();
      }
      
      lcd_gotoxy(1,1);
      printf(lcd_putc,"     Ingrese        ");
      lcd_gotoxy(1,2);
      printf(lcd_putc,"    Contraseńa      ");
      lcd_gotoxy(1,3);
      printf(lcd_putc,"    CLAVE=%i%i%i%i  ",clave[0],clave[1],clave[2],clave[3]);
      
      if(UP || BYTE_IR[2]==0X18)//Si oprime hacia arriba
      {
         if(Flanco == 0)
         {
            clave[unidad-11]++;Flanco = 1;delay_ms(30);t_latencia=0;BYTE_IR[2]=0X00;
         }
      }
      else
      {
         Flanco = 0;
      }
            
      if(DOWN || BYTE_IR[2]==0X52)//Si oprime hacia abajo
      {  
         if(Flanco2 == 0)
         {
            clave[unidad-11]--;Flanco2 = 1;delay_ms(30);t_latencia=0;BYTE_IR[2]=0X00;
         }
      }
      else
      {
         Flanco2 = 0;
      }
   
      if(RIGHT || BYTE_IR[2]==0X5A)// Si Oprime Derecha
      {
         if(Flanco1 == 0)
         {
            Flanco1 = 1;unidad++;delay_ms(30);printf(lcd_putc,"\f");t_latencia=0;BYTE_IR[2]=0X00;
         }
      }
      else
      {
         Flanco1 = 0;
      }
            
      if(LEFT || BYTE_IR[2]==0X08)// Si Oprime izquierda
      { 
         if(Flanco3 == 0)
         {
            Flanco3 = 1;unidad--;delay_ms(30);printf(lcd_putc,"\f");t_latencia=0;BYTE_IR[2]=0X00;
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
       {
         unidad=11;
       }
       lcd_gotoxy(unidad,4);// Para mostrar cursor.
       lcd_putc(t[2]);
       if(unidad>14)             // Si a Terminado de ingresar la clave, verifica si es correcta o no.
       {
         if(clave[0]==3&&clave[1]==8&&clave[2]==9&&clave[3]==2) // Si Ingresa clave para reset general del sistema.
            {write_eeprom(0,0);delay_ms(20);write_eeprom(1,0);delay_ms(20);// Reestablece a contraseńa de Fabrica y reinicia Programa.
             write_eeprom(2,0);delay_ms(20);write_eeprom(3,0);delay_ms(20);
             reset_cpu();}
            
         if(clave[0]==contrasena[0]&&clave[1]==contrasena[1]&&clave[2]==contrasena[2]&&clave[3]==contrasena[3]) // Si las claves coinciden pasa a Menu Principal.
            {lcd_gotoxy(1,1);
            printf(lcd_putc,"                   ");
            lcd_gotoxy(1,2);
            printf(lcd_putc,"     Contraseńa    ");
            lcd_gotoxy(1,3);
            printf(lcd_putc,"      Correcta     ");
            lcd_gotoxy(1,4);
            printf(lcd_putc,"                   ");
            delay_ms(500);Menu=50;unidad=11;printf(lcd_putc,"\f");flag_latencia=0;Latencia=0;tiempo_purga=1;
            t_latencia=0;flag_iniciodamper=1;}
         else                                         // Si la clave no coincide vuelve a mostrar el menu para ingresar la clave.
         {lcd_gotoxy(1,1);
            printf(lcd_putc,"");
            lcd_gotoxy(1,2);
            printf(lcd_putc,"     Contraseńa    ");
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
      {
         paso=0;
      }
   
      lcd_gotoxy(1,1);
      printf(lcd_putc,"---MENU PRINCIPAL---");
   
      if(UP || BYTE_IR[2]==0X18)//Si oprime hacia arriba
      {
         if(Flanco == 0)
         {
            Flecha2--;Flecha--;Flecha1=Flecha+1;Flanco = 1;delay_ms(30);BYTE_IR[2]=0X00;
         }
      }
      else
      {
         Flanco = 0;
      }
            
      if(DOWN || BYTE_IR[2]==0X52)//Si oprime hacia abajo
      {
         if(Flanco2 == 0)
         {
            Flecha2++;Flecha++;Flecha1=Flecha-1;Flanco2 = 1;delay_ms(30);BYTE_IR[2]=0X00;
         }
      }
      else
      {
         Flanco2 = 0;
      }
   
      if(Flecha2>nMenuH)
      {
         paso++;nMenuH=Flecha2;nMenuL=nMenuH-2;Flecha=4;
      }
        
      if(Flecha2<nMenuL)
      {
         paso--;nMenuL=Flecha2;nMenuH=nMenuL+2;Flecha=2;
      }
        
      if(Flecha2>n_opcionH)
      {
         Flecha2=n_opcionL;Flecha=2;paso=0;nMenuL=Flecha2;nMenuH=nMenuL+2;
      }
        
      if(Flecha2<n_opcionL)
      {
         Flecha2=n_opcionH;Flecha=4;paso=n_opcionH-4;nMenuH=Flecha2;nMenuL=nMenuH-2;
      }
        
      mensajes(1+paso,2);
      mensajes(2+paso,3);
      mensajes(3+paso,4);

      lcd_gotoxy(1,Flecha);// Para mostrar la flecha de seleccion
      lcd_putc(t[0]);

      if(Flecha==2)
      {
         lcd_gotoxy(1,4);// Para mostrar la flecha de seleccion
         lcd_putc(t[1]);
         lcd_gotoxy(1,3);// Para mostrar la flecha de seleccion
         lcd_putc(t[1]);
      }
        
      if(Flecha==4)
      {
         lcd_gotoxy(1,2);// Para mostrar la flecha de seleccion
         lcd_putc(t[1]);
         lcd_gotoxy(1,3);// Para mostrar la flecha de seleccion
         lcd_putc(t[1]);
      }
        
      if(Flecha==3)
      {
         lcd_gotoxy(1,4);// Para mostrar la flecha de seleccion
         lcd_putc(t[1]);
         lcd_gotoxy(1,2);// Para mostrar la flecha de seleccion
         lcd_putc(t[1]);
      }

      if(RIGHT || BYTE_IR[2]==0X1C)// Si oprime derecha
      {
         if(Flanco1 == 0)
         {
            Menu=Flecha2;Flecha=3;delay_ms(500);printf(lcd_putc,"\f");BYTE_IR[2]=0X00;
            if(Menu==6)
            {
               clave[0]=0;clave[1]=0;clave[2]=0;clave[3]=0;
            }
         }
      }
      else
      {
         Flanco1 = 0;
      }
            
      if(LEFT || BYTE_IR[2]==0X08)// Si oprime Izquierda
      {  
         if(Flanco3 == 0)
         {
            delay_ms(500);Menu=20;printf(lcd_putc,"\f");BYTE_IR[2]=0X00;
         }
      }
      else
      {
         Flanco3 = 0;
      }
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
                                                                    
   
      if(UP || BYTE_IR[2]==0X18)//Si oprime hacia arriba
      {
         delay_ms(200);
         printf(lcd_putc,"\f");
         lcd_gotoxy(1,2);
         printf(lcd_putc," Reset de tiempo ");
         lcd_gotoxy(1,3);
         printf(lcd_putc,"     Exitoso     ");
         write_eeprom(70,0);write_eeprom(71,0);write_eeprom(72,0);
         delay_ms(700);
         segundos=0;minutos=0;horas=0;
         delay_ms(30);Menu=1; paso=0;Flecha=2;Flecha2=2;printf(lcd_putc,"\f");
         BYTE_IR[2]=0X00;
      }
      
      if(RIGHT || BYTE_IR[2]==0X5A)
      {
         flag_filtro=!flag_filtro;write_eeprom(73,flag_filtro);
         delay_ms(500);Menu=1; paso=0;Flecha=2;Flecha2=2;printf(lcd_putc,"\f");BYTE_IR[2]=0X00;
      }
            
      if(LEFT || BYTE_IR[2]==0X08)// Si oprime Izquierda
      {  
         if(Flanco3 == 0)
         {
            Flanco3 = 1;delay_ms(500);Menu=1; paso=0;Flecha=2;Flecha2=2;printf(lcd_putc,"\f");BYTE_IR[2]=0X00;
         }
      }
      else
      {
         Flanco3 = 0;
      }
   }      
//----------------Fin-Menu3---------------------------------------------------------------

//----------------Menu4---------------------------------------------------------------
   if(Menu == 4){ // Menu de seleccion de SetPoint
   lcd_gotoxy(1,2);
   printf(lcd_putc,"      Setpoint      ");
   lcd_gotoxy(1,3);
   printf(lcd_putc,"       %2.2f   ",setpoint);
   
   if(setpoint>2.0)
   {
      setpoint=2.0;
   }
   
   if(setpoint<0.5)
   {
      setpoint=0.5;
   }
   
      if(UP || BYTE_IR[2]==0X18)//Si oprime hacia arriba
      {  
         if(Flanco == 0)
         {
            setpoint+=0.1;Flanco = 1;delay_ms(30);BYTE_IR[2]=0X00;
         }
      }
      else
      {
         Flanco = 0;
      }
            
      if(DOWN || BYTE_IR[2]==0X52)//Si oprime hacia abajo
      {  
         if(Flanco2 == 0)
         {
            setpoint-=0.1;Flanco2 = 1;delay_ms(30);BYTE_IR[2]=0X00;
         }
      }
      else
      {
         Flanco2 = 0;
      }
   
      if(RIGHT || BYTE_IR[2]==0X5A)// Si oprime derecha
      {  
         if(Flanco1 == 0)
         {
            Menu=1;paso=0;Flecha=2;Flecha2=2;unidad=11;delay_ms(500);printf(lcd_putc,"\f");
            Temporal=setpoint;
            Entero=(int)setpoint;
            Temporal=Temporal-Entero;
            Temporal2=Temporal*100.0;
            Decimal2=(int8)Temporal2;
            write_eeprom(40,Entero);write_eeprom(41,Decimal2);//Guardar valor de Setpoint en eeprom
            BYTE_IR[2]=0X00;
         }
      }
      else
      {
         Flanco1 = 0;
      }
            
      if(LEFT || BYTE_IR[2]==0X08)// Si oprime Izquierda
      {
         if(Flanco3 == 0)
         {
            Menu=1;paso=0;Flecha=2;Flecha2=2;unidad=11;delay_ms(500);printf(lcd_putc,"\f");BYTE_IR[2]=0X00;
         }
      }
      else
      {
         Flanco3 = 0;
      }
   }      
//----------------Fin-Menu4---------------------------------------------------------------

//----------------Menu5---------------------------------------------------------------
   if(Menu == 5){ // Menu para Cambio de Contraseńa
   lcd_gotoxy(1,1);
   printf(lcd_putc,"      Ingrese       ");
   lcd_gotoxy(1,2);
   printf(lcd_putc," Contraseńa Actual  ");
   lcd_gotoxy(1,3);
   printf(lcd_putc,"    CLAVE=%i%i%i%i  ",clave[0],clave[1],clave[2],clave[3]);
   
      
      if(UP || BYTE_IR[2]==0X18)//Si oprime hacia arriba
      {
         if(Flanco == 0)
         {
            clave[unidad-11]++;Flanco = 1;delay_ms(30);BYTE_IR[2]=0;
         }
      }
      else
      {
         Flanco = 0;
      }
            
      if(DOWN || BYTE_IR[2]==0X52)//Si oprime hacia abajo
      {  
         if(Flanco2 == 0)
         {
            clave[unidad-11]--;Flanco2 = 1;delay_ms(30);BYTE_IR[2]=0;
         }
      }
      else
      {
         Flanco2 = 0;
      }
   
      if(RIGHT || BYTE_IR[2]==0X5A)// Si oprime derecha
      {  
         if(Flanco1 == 0)
         {
            Flanco1 = 1;unidad++;delay_ms(500);printf(lcd_putc,"\f");BYTE_IR[2]=0;
         }
      }
      else
      {
         Flanco1 = 0;
      }
            
      if(LEFT || BYTE_IR[2]==0X08)// Si oprime Izquierda
      {
         if(Flanco3 == 0)
         {
            Flanco3 = 1;unidad--;delay_ms(500);printf(lcd_putc,"\f");BYTE_IR[2]=0;
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
       {Menu=1;paso=0;Flecha=2;Flecha2=2;unidad=11;}
       
       lcd_gotoxy(unidad,4);// Para mostrar la flecha de seleccion
       lcd_putc(t[2]);
       
       if(unidad>14)// Si ya ingreso la contraseńa muestra si es correcta o no, dependiendo si ingreso la clave correctamente.
       {
         if(clave[0]==contrasena[0]&&clave[1]==contrasena[1]&&clave[2]==contrasena[2]&&clave[3]==contrasena[3])
            {lcd_gotoxy(1,1);
            printf(lcd_putc,"                    ");
            lcd_gotoxy(1,2);
            printf(lcd_putc,"     Contraseńa     ");
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
            printf(lcd_putc,"     Contraseńa     ");
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
   if(Menu == 51){ // Menu cuando ingresa correctamente la contraseńa, permite que digite nueva contraseńa.
   lcd_gotoxy(1,1);
   printf(lcd_putc,"     Ingrese        ");
   lcd_gotoxy(1,2);
   printf(lcd_putc,"  Contraseńa Nueva  ");
   lcd_gotoxy(1,3);
   printf(lcd_putc,"    CLAVE=%i%i%i%i  ",clave[0],clave[1],clave[2],clave[3]);
   
      
      if(UP || BYTE_IR[2]==0X18)//Si oprime hacia arriba
      {  
         if(Flanco == 0)
         {
            clave[unidad-11]++;Flanco = 1;delay_ms(30);BYTE_IR[2]=0;
         }
      }
      else
      {
         Flanco = 0;
      }
            
      if(DOWN || BYTE_IR[2]==0X52)//Si oprime hacia abajo
      {  
         if(Flanco2 == 0)
         {
            clave[unidad-11]--;Flanco2 = 1;delay_ms(30);BYTE_IR[2]=0;
         }
      }
      else
      {
         Flanco2 = 0;
      }
   
      if(RIGHT || BYTE_IR[2]==0X5A)// Si oprime Derecha
      {
         if(Flanco1 == 0)
         {
            Flanco1 = 1;unidad++;delay_ms(30);printf(lcd_putc,"\f");BYTE_IR[2]=0;
         }
      }
      else
      {
         Flanco1 = 0;
      }
            
      if(LEFT || BYTE_IR[2]==0X08)// Si oprime Izquierda
      {
         if(Flanco3 == 0)
         {
            Flanco3 = 1;unidad--;delay_ms(30);printf(lcd_putc,"\f");BYTE_IR[2]=0;
         }
      }
      else
      {
         Flanco3 = 0;
      }
            
       if(clave[unidad-11]<0)// Si la unidad donde se encuentra ubicado el cursor es menor que 0 pasa a 9.     
       {clave[unidad-11]=9;}
       if(clave[unidad-11]>9)// Si la unidad donde se encuentra ubicado el cursor es mayor que 9 pasa a 0.     
       {clave[unidad-11]=0;}
       if(unidad<11)
       {Menu=5;unidad=11;}
       lcd_gotoxy(unidad,4);// Para mostrar la flecha de seleccion
       lcd_putc(t[2]);
       if(unidad>14)// Si ya ingreso la nueva contraseńa.
       {
            lcd_gotoxy(1,1);
            printf(lcd_putc,"                    ");
            lcd_gotoxy(1,2);
            printf(lcd_putc,"     Contraseńa     ");
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
   if(Menu == 6)
   {
   lcd_gotoxy(1,2);
   printf(lcd_putc,"     Media Movil    ");
   lcd_gotoxy(1,3);
   printf(lcd_putc,"       %02i   ",MediaMovil);
   
   if(MediaMovil>12)
   {MediaMovil=12;}
   
   if(MediaMovil<1)
   {MediaMovil=1;}
   
      if(UP || BYTE_IR[2]==0X18)//Si oprime hacia arriba
      {  
         if(Flanco == 0)
         {
            MediaMovil+=1;Flanco = 1;delay_ms(30);BYTE_IR[2]=0;
         }
      }
      else
      {
         Flanco = 0;
      }
            
      if(DOWN || BYTE_IR[2]==0X52)//Si oprime hacia abajo
      {  
         if(Flanco2 == 0)
         {
            MediaMovil-=1;Flanco2 = 1;delay_ms(30);BYTE_IR[2]=0;
         }
      }
      else
      {
         Flanco2 = 0;
      }
   
      if(RIGHT || BYTE_IR[2]==0X5A)// Si oprime derecha
      {  
         if(Flanco1 == 0)
         {
            Menu=1;paso=0;Flecha=2;Flecha2=2;unidad=11;delay_ms(500);printf(lcd_putc,"\f");
            write_eeprom(42,MediaMovil);//Guardar valor de Setpoint en eeprom
            BYTE_IR[2]=0;
         }
      }
      else
      {
         Flanco1 = 0;
      }
            
      if(LEFT || BYTE_IR[2]==0X08)// Si oprime Izquierda
      {
         if(Flanco3 == 0)
         {
            Menu=1;paso=0;Flecha=2;Flecha2=2;unidad=11;delay_ms(500);printf(lcd_putc,"\f");BYTE_IR[2]=0;
         }
      }
      else
      {
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
    
      if(RIGHT || BYTE_IR[2]==0X5A)// Si oprime derecha
      {  
         if(Flanco1 == 0)
         {
            Menu=1;paso=0;Flecha=2;Flecha2=2;unidad=11;delay_ms(500);printf(lcd_putc,"\f");
            Diferencia=zero_fabrica-sensores(0);
            
            if(Diferencia>=0)
            {
               negativo=10;write_eeprom(50,negativo);
            }
            if(Diferencia<0)
            {
               negativo=20;write_eeprom(50,negativo);
            }
            Diferencia=abs(Diferencia);
            Temporal1=Diferencia;
            Entero1=(int)Temporal1;
            Temporal1=Temporal1-Entero1;
            Temporal12=Temporal1*100.0;
            Decimal12=(int8)Temporal12;
            write_eeprom(60,Entero1);write_eeprom(61,Decimal12);//Guardar valor de Setpoint en eeprom
            BYTE_IR[2]=0;
         }
      }
      else
      {
         Flanco1 = 0;
      }
            
      if(LEFT || BYTE_IR[2]==0X08)// Si oprime Izquierda
      {
         if(Flanco3 == 0)
         {
            Menu=1;paso=0;Flecha=2;Flecha2=2;unidad=11;delay_ms(500);printf(lcd_putc,"\f");BYTE_IR[2]=0;
         }
      }
      else
      {
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
   printf(lcd_putc,"Ajuste1= %2.2f ",Ajuste1);
      
      if(UP || BYTE_IR[2]==0X18)//Si oprime hacia arriba
      {
         Ajuste1+=0.01;delay_ms(50);
         if(Ajuste1>30.0)
         {Ajuste1=0.0;}
         BYTE_IR[2]=0;
      }
            
      if(DOWN || BYTE_IR[2]==0X52)//Si oprime hacia abajo
      {
         Ajuste1-=0.01;delay_ms(50);
         if(Ajuste1<0.0)
         {Ajuste1=0.0;}
         BYTE_IR[2]=0;
      }
      
      if(RIGHT || BYTE_IR[2]==0X5A)// Si oprime derecha
      {
         if(Flanco1 == 0)
         {
            Menu=1;paso=0;Flecha=2;Flecha2=2;unidad=11;delay_ms(30);printf(lcd_putc,"\f");
            Temporal=Ajuste1;
            Entero=(int)Ajuste1;
            Temporal=Temporal-Entero;
            Temporal2=Temporal*100.0;
            Decimal1=(int8)Temporal2;
            write_eeprom(25,Entero);write_eeprom(26,Decimal1);
            delay_ms(1000);
            BYTE_IR[2]=0;
         }
      }
      else
      {
         Flanco1 = 0;
      }
            
      if(LEFT || BYTE_IR[2]==0X08)// Si oprime Izquierda
      {
         if(Flanco3 == 0)
         {
            Menu=1;paso=0;Flecha=2;Flecha2=2;unidad=11;delay_ms(500);printf(lcd_putc,"\f");BYTE_IR[2]=0;
         }
      }
      else
      {
         Flanco3 = 0;
      }
   }
//----------------Fin-Menu8---------------------------------------------------------------

//----------------Menu9---------------------------------------------------------------
   if(Menu == 9)// Menu de Modo
   {
      lcd_gotoxy(1,1);
      printf(lcd_putc,"Velocidad Motor=%2.0f%% ",(100*((float)tmp/255)));
   
      lcd_gotoxy(1,4);
      printf(lcd_putc,"InFlow= %2.1f(m/s) ",Flujo);
      
      ModoManual=0;ModoAuto=0;Automa=10;Manual=10;
      lcd_gotoxy(1,2);
      printf(lcd_putc," Control Velocidad  ");
      lcd_gotoxy(1,3);
      printf(lcd_putc," !Ajuste Setpoint!  ");
      
      if(UP || BYTE_IR[2]==0X18)
      {
         if(tmp<255)
         {
            tmp++;
            delay_ms(50);
         }
         BYTE_IR[2]=0X00;
      }
            
      if(DOWN || BYTE_IR[2]==0X52)
      {
         if(tmp>51)
         {
            tmp--;
            delay_ms(50);
         }
         BYTE_IR[2]=0X00;
      }
      
      ValorSemi=tmp;
      ajustar_damper(tmp);
      if(RIGHT || BYTE_IR[2]==0X5A)
      {
         write_eeprom(13,tmp);ModoSemi=0;Semi=20;Automa=10;Manual=10;delay_ms(500);printf(lcd_putc,"\f");
         write_eeprom(12,Semi);write_eeprom(11,Automa);write_eeprom(10,Manual);
         Menu=1;paso=0;Flecha=2;Flecha2=2;unidad=11;BYTE_IR[2]=0X00;
      } 
      
      if(LEFT || BYTE_IR[2]==0X08)
      {
         Menu=1;paso=0;Flecha=2;Flecha2=2;unidad=11;delay_ms(500);printf(lcd_putc,"\f");BYTE_IR[2]=0X00;
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
      
      if(UP || BYTE_IR[2]==0X18)
      {
         if(unidad2==1)
         {
            if(purga_minutos_pro>59)
            {
               purga_minutos_pro=0;
            }
            else
            {
               purga_minutos_pro++;delay_ms(300);
            }
         }
         
         if(unidad2==3)
         {
            if(purga_segundos_pro>59)
            {
               purga_segundos_pro=0;
            }
            else
            {
               purga_segundos_pro++;delay_ms(300);
            }
         }
         BYTE_IR[2]=0X00;
      }
      
      if(DOWN || BYTE_IR[2]==0X52)
      {
         if(unidad2==1)
         {
            if(purga_minutos_pro<0)
            {
               purga_minutos_pro=59;
            }
            else
            {
               purga_minutos_pro--;delay_ms(300);
            }
         }
         
         if(unidad2==3)
         {
            if(purga_segundos_pro<0)
            {
               purga_segundos_pro=59;
            }
            else
            {
               purga_segundos_pro--;delay_ms(300);
            }
         }
         BYTE_IR[2]=0X00;
      }
      
      if(RIGHT || BYTE_IR[2]==0X5A)
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
         BYTE_IR[2]=0X00;
      }
      
      if(LEFT || BYTE_IR[2]==0X08)
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
         purga_minutos=purga_minutos_pro;purga_segundos=purga_segundos_pro;
         delay_ms(700);
         delay_ms(30);Menu=1; paso=0;Flecha=2;Flecha2=2;printf(lcd_putc,"\f");
         BYTE_IR[2]=0X00;
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
      
      if(UP || BYTE_IR[2]==0X18)
      {
         if(unidad2==1)
         {
            if(ppurga_minutos_pro>59)
            {
               ppurga_minutos_pro=0;
            }
            else
            {
               ppurga_minutos_pro++;delay_ms(300);
            }
         }
         
         if(unidad2==3)
         {
            if(ppurga_segundos_pro>59)
            {
               ppurga_segundos_pro=0;
            }
            else
            {
               ppurga_segundos_pro++;delay_ms(300);
            }
         }
         BYTE_IR[2]=0X00;
      }
      
      if(DOWN || BYTE_IR[2]==0X52)
      {
         if(unidad2==1)
         {
            if(ppurga_minutos_pro<0)
            {
               ppurga_minutos_pro=59;
            }
            else
            {
               ppurga_minutos_pro--;delay_ms(300);
            }
         }
         
         if(unidad2==3)
         {
            if(ppurga_segundos_pro<0)
            {
               ppurga_segundos_pro=59;
            }
            else
            {
               ppurga_segundos_pro--;delay_ms(300);
            }
         }
         BYTE_IR[2]=0X00;
      }
      
      if(RIGHT || BYTE_IR[2]==0X5A)
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
         BYTE_IR[2]=0X00;
      }
      
      if(LEFT || BYTE_IR[2]==0X08)
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
         ppurga_minutos=ppurga_minutos_pro;ppurga_segundos=ppurga_segundos_pro;
         delay_ms(700);
         delay_ms(30);Menu=1; paso=0;Flecha=2;Flecha2=2;printf(lcd_putc,"\f");
         BYTE_IR[2]=0X00;
      }  
       
   }
//----------------Fin-Menu11---------------------------------------------------------------

//----------------Menu12---------------------------------------------------------------
   if(Menu == 12)// Menu de Vidrio
   {
      lcd_gotoxy(1,1);
      printf(lcd_putc,"   Mover   Vidrio    ");
      
      if(UP)
      {
         lcd_gotoxy(1,3);
         printf(lcd_putc,"   !Subiendo!    ");   
          
         if(!VIDRIOUP || VIDRIODN)
         {
            O1_off;O2_on;
         }
         else
         {
            O1_off;O2_off;
         }
         BYTE_IR[3]=0X00;
         BYTE_IR[2]=0X00;
      }
      else
      {
         if(DOWN)
         {
            lcd_gotoxy(1,3);
            printf(lcd_putc,"   !Bajando!    ");   

            if(VIDRIOUP || !VIDRIODN)
            {
               O1_on;O2_off;
            }
            else
            {
               O1_off;O2_off;
            }
         }
         else
         {
            lcd_gotoxy(1,3);
            printf(lcd_putc,"   !Detenido!   ");   
            O1_off;O2_off;
         }
         BYTE_IR[3]=0X00;
         BYTE_IR[2]=0X00;
      }
      
      
      if(LEFT || BYTE_IR[2]==0X08)
      {
         delay_ms(500);
         printf(lcd_putc,"\f");
         Menu=1; paso=0;Flecha=2;Flecha2=2;printf(lcd_putc,"\f");
         BYTE_IR[2]=0X00;
      }  
       
   }
//----------------Fin-Menu12---------------------------------------------------------------


//-------------------Menu20----------------------------------------------------------------  

   if(Menu==20)
   {
      if(Lectura2==1)
      {
      lcd_gotoxy(1,1);                     
      printf(lcd_putc," JP CEGH");//T= %2.0f",sensores(1));//T= %2.1f",TC);
      lcd_gotoxy(1,2);
      printf(lcd_putc,"Ext=%2.0f%%",(100*((float)tmp/255)));
      //printf(lcd_putc,"Ext= %03u%%",FeedbackDamper);
      lcd_gotoxy(1,3);
      printf(lcd_putc,"InFlow= %2.1f (m/s) ",Flujo);
      Lectura2=0;
      }
      
      if(flag_filtro==1)
      {
         if(horas>=60)
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
         
      if(Flujo<0.5)
      {
         lcd_gotoxy(11,2);
         printf(lcd_putc,"Flujo=Bajo");      
      }
      else
      {
         if(Flujo>2.0)
         {
            lcd_gotoxy(11,2);
            printf(lcd_putc,"Flujo=Alto");               
         }
         else
         {
            lcd_gotoxy(11,2);
            printf(lcd_putc,"Flujo=OK  ");      
         }
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
            Rele_off;
         }
         lcd_gotoxy(10,4);
         printf(lcd_putc,"Motor:OFF "); 
      }
      
      if(DOWN || BYTE_IR[2]==0X45)//Si oprime hacia Abajo
      {  
         delay_ms(200);
         if(DOWN || BYTE_IR[2]==0X45)//Si oprime hacia Abajo
         {  
            if(Flanco == 0)
            {
               Flanco = 1;estadofl=!estadofl;BYTE_IR[2]=0X00;
            }
         }
      }
      else
      {
         Flanco = 0;
      }
            
      if(LEFT || BYTE_IR[2]==0X46)//Si oprime hacia Izquierda
      {  
         delay_ms(200);
         if(LEFT || BYTE_IR[2]==0X46)//Si oprime hacia Izquierda
         {     
            if(Flanco2 == 0)
            {
               Flanco2 = 1;estadomv=!estadomv;
               if(estadomv==1)
               {
                  Rele_on;
                  Motor_on;
                  delay_ms(150);
                  lcd_init();
               }
               else
               {
                  Motor_off;
                  Rele_off;
                  delay_ms(150);
                  lcd_init();
               }
               BYTE_IR[2]=0X00;
            }
         }
      }
      else
      {
         Flanco2 = 0;
      }
   
      if(UP || BYTE_IR[2]==0X1C)// Si oprime Arriba
      {  
         delay_ms(200);
         if(UP || BYTE_IR[2]==0X1C)// Si oprime Arriba
         {  
            if(Flanco1 == 0)
            {
               printf(lcd_putc,"\f");Menu=1;paso=0;Flecha=2;Flecha2=2;unidad=11;BYTE_IR[2]=0X00;
            }
         }
      }
      else
      {
         Flanco1 = 0;
      }
            
      if(RIGHT || BYTE_IR[2]==0X47)// Si oprime Derecha
      {
         delay_ms(200);
         if(RIGHT || BYTE_IR[2]==0X47)// Si oprime Derecha
         {
            if(Flanco3 == 0)
            {
               printf(lcd_putc,"\f");Menu=40;BYTE_IR[2]=0X00;
            }
         }
      }
      else
      {
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
         Rele_on;
         Motor_on;
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
  
   
      if(RIGHT || BYTE_IR[2]==0X5A)// Si oprime derecha
      {  
         delay_ms(100);
         if(RIGHT || BYTE_IR[2]==0X5A)// Si oprime derecha
         {  
            if(Flanco1 == 0)
            {
               Flanco1 = 1;Menu=30;intensidadpantalla(contraste);Alarma_off;printf(lcd_putc,"\f");tiempo_ppurga=1;BYTE_IR[2]=0X00;
            }
         }
      }
      else
      {
         Flanco1 = 0;
      }
            
      if(LEFT || BYTE_IR[2]==0X08)// Si oprime Izquierda
      {
         delay_ms(100);
         if(LEFT || BYTE_IR[2]==0X08)// Si oprime Izquierda
         {
            if(Flanco3 == 0)
            {
               Flanco3 = 1;Menu=20;paso=0;Flecha=2;Flecha2=2;unidad=11;delay_ms(500);printf(lcd_putc,"\f");BYTE_IR[2]=0X00;
            }
         }
      }
      else
      {
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
       
   if(LEFT || BYTE_IR[2]==0X08)
   {
      delay_ms(1000);
      if(LEFT || BYTE_IR[2]==0X08)
      {
         tiempo_purga=0;Motor_on;Menu=20;BYTE_IR[2]=0X00;Rele_on;
      }
   }
   if(Lectura==1)
   {
        Flujo=Leer_Sensor_Flujo(MediaMovil);
        controlar_flujo();    
        Motor_on;
        Rele_on;
        estadomv=1;
        Luz_Blanca_off;
   }
   }      
//----------------Fin-Menu50---------------------------------------------------------------  

   if(tiemporeset>=20)
   {
      lcd_init();
      tiemporeset=0;
   } 

   if(Menu!=0 || Menu!=30 || Menu!=200)
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
      if(Menu!=200)
      {
         intensidadpantalla(contraste);Alarma_off;Latencia=0;t_latencia=0;flag_latencia=0;
      }
      else
      {
         intensidadpantalla(0);
      }
   }

   }       
}
int8 BIT_BYTE(int1 D0, int1 D1, int1 D2, int1 D3, int1 D4, int1 D5, int1 D6, int1 D7){
   int8 dato;
   dato= D7*128 + D6*64 + D5*32 + D4*16 + D3*8 + D2*4 + D1*2 +D0*1; // 0 al 255 
   return dato; 
}
