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
#fuses HSPLL,NOWDT,NOPROTECT,NOLVP,NODEBUG,USBDIV,PLL1,CPUDIV4,NOVREGEN,NOMCLR// PLL1 para 4 MHz
#use delay(clock=12000000)
#include <LCD420.c>
#include <math.h>

// Definición de otros puertos

#define   Luz_Blanca_on     output_bit(PIN_B4,1)
#define   Luz_Blanca_off    output_bit(PIN_B4,0)

int8 Menu=100, n_opcionH=8,n_opcionL=2,unidad=11,Flecha=2,Flecha1=3,Flecha2=2, nMenuL=2, nMenuH=4,contraste=10;
int8 Entero=0,Decimal1=0,Decimal2=0,Entero1=0,Decimal12=0,n=0,l=0,h=0,tmp=10,MediaMovil=0,negativo=10,Automa=10,Manual=10,Semi=10,ValorSemi=0;
short estadofl=0,estadomv=0,estadoalarma=0,estadopantalla=0,Flanco=0,Flanco1=0,Flanco2=0,Flanco3=0,flag_latencia=0,Latencia=0;
signed int8   paso=0;
char t[3]={'>',' ','^'}; 
signed  int8 clave[4]={0,0,0,0};   // 0=Verdadero, 1=Falso
int8 contrasena[4]={0,0,0,0};   // 0=Verdadero, 1=Falso
float setpoint=0.0,zero=0,zero_fabrica=203.0;
short ModoSleep,inicio=0,Lectura=0,Lectura2=0,flag_iniciodamper=0,iniciodamper=0,ModoManual=0,ModoAuto=0,ModoSemi=0;
int16 tiempos=0,t_latencia=0,tiempos2=0,t_iniciodamper=0;
float Beta=3550,R25=1350,T0=298.15,TK=0,TC=0,Rdivisor=1000,Rntc=0.0,Voltaje0=0.0,Voltaje1=0.0,mV=0.0,Presion=0.0;
//float a=24.9616,b=-175.3954,c=489.5268,d=-440.7857;
 
//float a=-0.00000007109886647647915,b=0.0003576135181122329,c= -0.169922240146291,d= 19.248973132140719;//Sensor 1 Posible dañado
float a=0.0000001418633343546420,b=-0.00009476749001431169,c= 0.040182822903506,d= -5.354852229527197;//Sensor 2 Posible bueno
float Flujo=0.0,Flujo2=0.0,x=0.0,x_uno=0.0,x_cuadrado=0.0,x_cubo=0.0,K=0.2246,Temporal=0.0,Temporal2=0.0,Velocidad=0.0;
float Temporal1=0.0,Temporal12=0.0,Diferencia=0.0,Ajuste1=1.0;
float Tcalibracion=656.0,correccion=0.0,error=0.0;
float promedio[11];

// Funcion para conversor analogo-digital
float sensores(int x){
float y;set_adc_channel(x);delay_ms(10);y=read_adc();return (y);
}

Float Leer_Sensor_Flujo(int media)
{
    Voltaje0=sensores(0);
    Voltaje1=sensores(1);
    //Diferencia=(Voltaje0-Voltaje1)*(0.06/1023);
    Diferencia=(Voltaje0)*(5.0/1023.0);
    
    //if(Diferencia<0.0)
     //{Diferencia=0.0;}
     
     if(l>=media)
       {l=0;}
     
     promedio[l]=Diferencia;l++;
     Flujo2=0;
        for(h=0;h<=(media-1);h++)
        {
           Flujo2+=promedio[h];
        } 
           Flujo2=(Flujo2/media); 
           //return Flujo2;
           return Flujo2;
}

void main ()
{
   lcd_init();
   //setup_adc_ports(AN0_TO_AN1|VREF_VREF);
   setup_adc_ports(AN0_TO_AN1);
   setup_adc(ADC_CLOCK_DIV_32 );   
  
   while(true){
   mV=Leer_Sensor_Flujo(10);
   //Presion=(mV*1000)/2.66;
   lcd_gotoxy(2,1);printf(lcd_putc,"DIF:%03.6f mV ",mV);
   lcd_gotoxy(2,2);printf(lcd_putc,"CH0:%03.3f ADC ",Voltaje0);
   lcd_gotoxy(2,3);printf(lcd_putc,"CH1:%03.3f ADC ",Voltaje1);
   lcd_gotoxy(2,4);printf(lcd_putc,"PRE:%03.3f kPa ",Presion);
   delay_ms(1000);
   }
}
