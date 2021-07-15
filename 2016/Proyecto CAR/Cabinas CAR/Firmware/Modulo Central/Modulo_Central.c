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

// Definición de Entradas
#define   Cabina1         input(PIN_A0)
#define   Cabina2         input(PIN_A1)
#define   Cabina3         input(PIN_A2)
#define   Cabina4         input(PIN_A3)
#define   Cabina5         input(PIN_A5)
#define   Cabina6         input(PIN_E0)
#define   Cabina7         input(PIN_E1)
#define   Cabina8         input(PIN_E2)
#define   Cabina9         input(PIN_C0)
#define   Cabina10        input(PIN_C1)

// Definición de otros puertos

#define   Cabina_on           output_bit(PIN_D0,1)
#define   Cabina_off          output_bit(PIN_D0,0)
#define   Scrubber_on         output_bit(PIN_D1,1)
#define   Scrubber_off        output_bit(PIN_D1,0)


void main ()
{
   setup_adc_ports(NO_ANALOGS);
   output_d(0); 
  
   while(true){
      
      if(Cabina1 || Cabina2 || Cabina3 || Cabina4 || Cabina5 || Cabina6 || Cabina7 || Cabina8 || Cabina9 || Cabina10)
      {
         Cabina_on;
         Scrubber_on;
      }
      else
      {
         Cabina_off;
         Scrubber_off;
      }
   }
}
