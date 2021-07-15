// Reloj con XT de 4 MHz.
// Programación para Cabina de Bioseguridad A2
// Esclavo controlador de Hardware
// Ing. Juan David Piñeros Espinosa.
// JP Inglobal. 2016
// Pin A2, Analogo Damper
#include <18F4550.h>
#device adc=10
#fuses HSPLL,NOWDT,NOPROTECT,NOLVP,NODEBUG,USBDIV,PLL1,CPUDIV1,VREGEN,NOMCLR
#use delay(clock=48000000)
#include <math.h>
#use i2c(slave, fast, sda=PIN_B0, scl=PIN_B1, address=0xA0)

#define   UP               input(PIN_D2)
#define   DOWN             input(PIN_D3)

// Definición de otros puertos
#define   LuzBlanca_on           output_bit(PIN_B2,1)
#define   LuzBlanca_off          output_bit(PIN_B2,0)
#define   LuzUV_on               output_bit(PIN_B3,1)
#define   LuzUV_off              output_bit(PIN_B3,0)
#define   Motor_on               output_bit(PIN_B4,1)
#define   Motor_off              output_bit(PIN_B4,0)
#define   Toma_on                output_bit(PIN_B5,1)
#define   Toma_off               output_bit(PIN_B5,0)
#define   Alarma_on              output_bit(PIN_B6,1)
#define   Alarma_off             output_bit(PIN_B6,0)
#define   Motor1_off             output_bit(PIN_D0,0)
#define   Motor1_on              output_bit(PIN_D0,1)
#define   Motor2_off             output_bit(PIN_D1,0)
#define   Motor2_on              output_bit(PIN_D1,1)

byte fstate;                     //Guardara el estado del bus I2C
byte posicion, buffer[0x10], txbuf[0x11],rxbuf[0x11];     //Buffer de memoria
short EnvioMaster;               //Indicación de fin del byte enviado por el master
short SolicitudMaster=0;         //Indicación de fin de la entrega del byte solicitado por el master
int16 tiempos=0,tiempos2=0;
// Variables para Pt100
short capturar=0;
int8 h=0,l=0,posicionvector=0,r=0,q=0,tmp=0;
float V0=0.0,V2=0.0,V3=0.0,RPM=0.0,pH=0.0,OD=0.0;
float R1=6200.0,Pt,Prom=0.0,Temperature=0.0;
float promedio[80]={0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
float pulsos1=0;
float rpm_actual=0.0,n_pulsos=512.0,Inflowpic=0.0;
unsigned int16 pwm1=0,pwm2=0;
unsigned int8 Inflow_l=0,Inflow_h=0,Downflow_l=0,Downflow_h=0,Vidrio_abajo=0,Vidrio_ok=0,Filtro_Inflow_l=0,Filtro_Inflow_h=0;
unsigned int8 Filtro_Downflow_l=0,Filtro_Downflow_h=0;
unsigned int16 Inflow=0.0,Downflow=0.0,Filtro_Inflow=0.0,Filtro_Downflow=0.0;

// Funcion para conversor analogo-digital
unsigned int16 sensores(int x){
unsigned int16 y;set_adc_channel(x);delay_ms(10);y=read_adc();return (y);
}

#INT_SSP
void ssp_interupt (){
   
   int incoming;                //Variable donde se recibe el byte que manda el maestro
   
   fstate = i2c_isr_state();    //Lectura del estado del bus I2c la interrupción

   /* Solicitud de lectura del esclavo por el master */
   if(fstate == 0x80) {         
       //Manda al maestro la información contenida en la posición de memoria que le ha solicitado
      i2c_write (txbuf[posicion]);
      //i2c_write (txbuf[0]);
      output_toggle(PIN_B7);
      //i2c_write ((int8)V2);
   }
   /* Sino está solicitando lectura es que está enviando algo */
   else {                              //Sino es que hay dato en el bus I2C...
      incoming = i2c_read();           //... lo lee
   

      if (fState == 1) {          //Información recibida corresponde a la posicion
         posicion = incoming;          //Se guarda posición
      }
      else if (fState == 2) {          //Información recibida corresponde al dato
         rxbuf[posicion] = incoming;
      }
  }
}

void controlar_flujo(void)
{
   Inflowpic=rxbuf[5]+(rxbuf[6]/100.0);
   if(rxbuf[2]==5)
   {
      if(Inflowpic<0.3)
      {
         tmp+=1;
      }
         
      if(Inflowpic>0.5)
      {
         tmp-=1;
      }         
         
      if(tmp>251)
      {tmp=251;}
      if(tmp<4)         
      {tmp=4;}      
      set_pwm1_duty (tmp);
   }
}

#int_TIMER1
void temp1s(void){
   set_timer1(5536);
   tiempos++;
   tiempos2++;
 
   if(tiempos>=20)
   {
      capturar=1;tiempos=0;
   }
   
   if(tiempos2>=200)
   {
      tiempos2=0;controlar_flujo();
   }
}

void LeerInflow(){
   Inflow=sensores(0);
   Inflow_l=(int8)Inflow;
   Inflow_h=(Inflow>>8);
   txbuf[0]=(int8)Inflow_l;
   txbuf[1]=(int8)Inflow_h;
}

void LeerDownflow(){
   Downflow=sensores(2);
   Downflow_l=(int8)Downflow;
   Downflow_h=(Downflow>>8);
   txbuf[2]=(int8)Downflow_l;
   txbuf[3]=(int8)Downflow_h;
}

void LeerVidrioAbajo(){
   if(sensores(4)<696)
      txbuf[4]=5;
   else
      txbuf[4]=10;
}

void LeerVidriook(){
   if(sensores(5)<820)
      txbuf[5]=5;
   else
      txbuf[5]=10;
}

void LeerFiltroInflow(){
   Filtro_Inflow=sensores(6);
   Filtro_Inflow_l=(int8)Filtro_Inflow;
   Filtro_Inflow_h=(Filtro_Inflow>>8);
   txbuf[6]=(int8)Filtro_Inflow_l;
   txbuf[7]=(int8)Filtro_Inflow_h;
}

void LeerFiltroDownflow(){
   Filtro_Downflow=sensores(7);
   Filtro_Downflow_l=(int8)Filtro_Downflow;
   Filtro_Downflow_h=(Filtro_Downflow>>8);
   txbuf[8]=(int8)Filtro_Downflow_l;
   txbuf[9]=(int8)Filtro_Downflow_h;
}


void main (){

   fState = 0;
   for (posicion=0;posicion<0x10;posicion++)
   {
      buffer[posicion] = 0x00;
      txbuf[posicion] = 0x00;
      rxbuf[posicion] = 0x00;
   }

   setup_timer_0(RTCC_DIV_1|RTCC_EXT_L_TO_H);
   setup_timer_1(T1_INTERNAL|T1_DIV_BY_1);
   setup_timer_2(T2_DIV_BY_4, 255, 1); 
   setup_ccp1 (CCP_PWM);
   enable_interrupts(INT_TIMER1);
   enable_interrupts(INT_SSP);
   setup_adc_ports(AN0_TO_AN7);
   setup_adc(ADC_CLOCK_DIV_32 );
   enable_interrupts(global);
   set_pwm1_duty(128);
      
   while(true){
     
     if(DOWN)
     {
         if(txbuf[4]==10)
         {
            Motor1_on;Motor2_off;
         }
         else
         {
            Motor1_off;Motor2_off;
         }
     }
     else
     {
        if(UP)
        {
            if(txbuf[5]==10)
            {
               Motor1_off;Motor2_on;   
            }
            else
            {
               Motor1_off;Motor2_off;
            }
        }    
        else
        {
            Motor1_off;Motor2_off;   
        }
     }
     
     
      
     if(rxbuf[0]==5) //Luz Blanca 
      {
         LuzBlanca_on;
      }
      if(rxbuf[0]==10) //Luz Blanca
      {
         LuzBlanca_off;
      }
      
      if(rxbuf[1]==5) //Luz UV
      {
         LuzUV_on;
      }
      if(rxbuf[1]==10) //Luz UV
      {
         LuzUV_off;
      }
      
      if(rxbuf[2]==5) //Motor
      {
         Motor_on;
      }
      if(rxbuf[2]==10) //Motor
      {
         Motor_off;
      }
      
      if(rxbuf[3]==5) //Toma
      {
         Toma_on;
      }
      if(rxbuf[3]==10) //Toma
      {
         Toma_off;
      }
      
      if(rxbuf[4]==5) //Alarma
      {
         Alarma_on;
      }
      if(rxbuf[4]==10) //Alarma
      {
         Alarma_off;
      }
      
      if(capturar==1)
      {
         capturar=0;       
         LeerDownflow();
         LeerFiltroDownflow();
         LeerFiltroInflow();
         LeerInflow();
         LeerVidrioAbajo();
         LeerVidriook();   
      }
   }
}

