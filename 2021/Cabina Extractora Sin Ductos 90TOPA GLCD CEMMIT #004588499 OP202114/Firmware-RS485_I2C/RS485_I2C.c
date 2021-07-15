// Cuatro pulsadores de entrada (Derecha, Izquierda, arriba y abajo).
// Reloj con XT de 4 MHz.
// Programación para Autoclave con Calderin
// Tiene Menú:Ciclo: Liquidos, Instrumental suave, Textil Instrumental y Caucho.
// Tiene 6 niveles programables para desfogue suave.
// Ing. Juan David Piñeros.
// JP Inglobal. 2015

#include <18F2550.h>
#fuses XTPLL,WDT1024,NOPROTECT,NOLVP,NODEBUG,USBDIV,PLL1,CPUDIV4,VREGEN,NOSTVREN,NOPUT,NOCPD,NOWRT,NOIESO,NOFCMEN,NOPBADEN,NOWRTC,NOWRTB,NOEBTR,NOEBTRB,NOCPB,NOXINST,NOBROWNOUT,NOLPT1OSC,NOMCLR
//#fuses HSPLL,NOWDT,NOPROTECT,NOLVP,NODEBUG,USBDIV,PLL1,CPUDIV1,VREGEN,NOMCLR
#use delay(clock=16000000)

#use i2c(slave, slow, sda=PIN_B0, scl=PIN_B1, address=0xB0)

#define   Buzzer_on           output_bit(PIN_A5,1)
#define   Buzzer_off          output_bit(PIN_A5,0)

#define MODBUS_TYPE MODBUS_TYPE_MASTER
#define MODBUS_SERIAL_TYPE MODBUS_RTU     //use MODBUS_ASCII for ASCII mode
#define MODBUS_SERIAL_RX_BUFFER_SIZE 1023
#define MODBUS_SERIAL_BAUD 9600

//#use RS232(BAUD=9600, BITS=8, PARITY=N, XMIT=PIN_B4, RCV=PIN_B5,stream=PC, errors)
#define MODBUS_SERIAL_INT_SOURCE MODBUS_INT_RDA
#define MODBUS_SERIAL_TX_PIN PIN_C6   // Data transmit pin
#define MODBUS_SERIAL_RX_PIN PIN_C7   // Data receive pin
//The following should be defined for RS485 communication
#define MODBUS_SERIAL_ENABLE_PIN   PIN_B2   // Controls DE pin for RS485
#define MODBUS_SERIAL_RX_ENABLE    0   // Controls RE pin for RS485
//#define DEBUG_MSG(msg) fprintf(PC, msg)
//#define DEBUG_DATA(msg,data) fprintf(PC, msg, data)


#include <modbus.c>

#define MODBUS_SLAVE_ADDRESS 0x01

// Variables para Pt100
int8 instruccion=0,address=0,regl=0,regh=0,i=0,pwm1l=0,pwm1h=0,pwm2l=0,pwm2h=0,address_ant=0,reg_ant=0,crc1=0,crc2=0,conteoError=0;
int16 reg=0,pwm1=0,pwm2=0,temporal=0;

int8 fstate;                     //Guardara el estado del bus I2C
int8 posicion, buffer[50], txbuf[50],rxbuf[50];     //Buffer de memoria

#INT_SSP
void ssp_interupt (){
   int8 incoming;                //Variable donde se recibe el byte que manda el maestro
   
   fstate = i2c_isr_state();    //Lectura del estado del bus I2c la interrupción

   if(fstate == 0x80) {         
       //Manda al maestro la información contenida en la posición de memoria que le ha solicitado
      i2c_write (txbuf[posicion]);
   }else {                              //Sino es que hay dato en el bus I2C...
      incoming = i2c_read();           //... lo lee
      if (fState == 1) {          //Información recibida corresponde a la posicion
         posicion = incoming;          //Se guarda posición
      }
      else if (fState == 2) {          //Información recibida corresponde al dato
         rxbuf[posicion] = incoming;
      }
  }
}

void Lee_Vector(void){
   instruccion=rxbuf[0];
   address=rxbuf[1];
   regl=rxbuf[2];
   regh=rxbuf[3];
   reg=(regh*256)+regl;
   pwm1l=rxbuf[4];
   pwm1h=rxbuf[5];
   pwm2l=rxbuf[6];
   pwm2h=rxbuf[7];
   pwm1=(pwm1h*256)+pwm1l;
   pwm2=(pwm2h*256)+pwm2l;
   crc1=rxbuf[8];
   crc2=rxbuf[9];
}

int8 swap_bits(int8 c)
{
   return ((c&1)?128:0)|((c&2)?64:0)|((c&4)?32:0)|((c&8)?16:0)|((c&16)?8:0)
          |((c&32)?4:0)|((c&64)?2:0)|((c&128)?1:0);
}

void read_all_coils()
{
   //DEBUG_MSG("Coils:\r\n");
   if(!(modbus_read_coils(MODBUS_SLAVE_ADDRESS,0,8)))
   {
      //DEBUG_MSG("Data: ");
      //for(i=1; i < (modbus_rx.len); ++i)
         //DEBUG_DATA("%X ", modbus_rx.data[i]);
      //DEBUG_MSG("\r\n\r\n");
   }
   else
   {
      //DEBUG_DATA("<-**Exception %X**->\r\n\r\n", modbus_rx.error);
   }
}

void read_all_inputs()
{
   //DEBUG_MSG("Inputs:\r\n");
   if(!(modbus_read_discrete_input(MODBUS_SLAVE_ADDRESS,0,8)))
   {
      //DEBUG_MSG("Data: ");
      //for(i=1; i < (modbus_rx.len); ++i)
      //   DEBUG_DATA("%X ", modbus_rx.data[i]);
      //DEBUG_MSG("\r\n\r\n");
   }
   else
   {
      //DEBUG_DATA("<-**Exception %X**->\r\n\r\n", modbus_rx.error);
   }
}

void read_all_holding(int8 start,int8 size)
{
   //DEBUG_MSG("Holding Registers:\r\n");
   if(!(modbus_read_holding_registers(MODBUS_SLAVE_ADDRESS,start,size)))
   {
      //DEBUG_MSG("Data: ");
      for(i=1; i < (modbus_rx.len); ++i){
         //DEBUG_DATA("%X ", modbus_rx.data[i]);
         if(i<20)
            txbuf[i]=(int8)modbus_rx.data[i];
      }
      //DEBUG_MSG("\r\n\r\n");
      conteoError=0;
   } 
   else
   {
      //DEBUG_DATA("<-**Exception %X**->\r\n\r\n", modbus_rx.error);
      conteoError++;
      if(conteoError>20){
         for(i=1; i < 20; ++i){         
            txbuf[i]=0;
         }
      }
   }
}

void read_all_input_reg(int8 start,int8 size)
{
   //DEBUG_MSG("Input Registers:\r\n");
   if(!(modbus_read_input_registers(MODBUS_SLAVE_ADDRESS,start,size)))
   {
      //DEBUG_MSG("Data: ");      
      //for(i=1; i < (modbus_rx.len); ++i)
         //DEBUG_DATA("%X ", modbus_rx.data[i]);       
       
       //DEBUG_MSG("\r\n\r\n");

   }
   else
   {
      //DEBUG_DATA("<-**Exception %X**->\r\n\r\n", modbus_rx.error);
   }
}

void write_coil()
{
   //DEBUG_MSG("Writing Single Coil:\r\n");
   if(!(modbus_write_single_coil(MODBUS_SLAVE_ADDRESS,6,TRUE)))
   {
      //DEBUG_MSG("Data: ");
      //for(i=0; i < (modbus_rx.len); ++i)
      //   DEBUG_DATA("%X ", modbus_rx.data[i]);
      //DEBUG_MSG("\r\n\r\n");
   }
   else
   {
      //DEBUG_DATA("<-**Exception %X**->\r\n\r\n", modbus_rx.error);
   }   
}

void write_reg(int16 address,int16 reg)
{
   //DEBUG_MSG("Writing Single Register:\r\n");
   if(!(modbus_write_single_register(MODBUS_SLAVE_ADDRESS,address,reg)))
   {
      //DEBUG_MSG("Data: ");
      //for(i=0; i < (modbus_rx.len); ++i)
         //DEBUG_DATA("%X ", modbus_rx.data[i]);
      //DEBUG_MSG("\r\n\r\n");
   }
   else
   {
      //DEBUG_DATA("<-**Exception %X**->\r\n\r\n", modbus_rx.error);
   }
}

void write_coils()
{
   int8 coils[1] = { 0x50 };
   //DEBUG_MSG("Writing Multiple Coils:\r\n");
   if(!(modbus_write_multiple_coils(MODBUS_SLAVE_ADDRESS,0,8,coils)))
   {
      //DEBUG_MSG("Data: ");
      //for(i=0; i < (modbus_rx.len); ++i)
      //   DEBUG_DATA("%X ", modbus_rx.data[i]);
      //DEBUG_MSG("\r\n\r\n");
   }
   else
   {
      //DEBUG_DATA("<-**Exception %X**->\r\n\r\n", modbus_rx.error);
   }   
}

void write_regs()
{
   int16 reg_array[2] = {0x1111, 0x2222};
   //DEBUG_MSG("Writing Multiple Registers:\r\n");
   if(!(modbus_write_multiple_registers(MODBUS_SLAVE_ADDRESS,0,2,reg_array)))
   {
      //DEBUG_MSG("Data: ");
      //for(i=0; i < (modbus_rx.len); ++i)
      //   DEBUG_DATA("%X ", modbus_rx.data[i]);
      //DEBUG_MSG("\r\n\r\n");
   }
   else
   {
      //DEBUG_DATA("<-**Exception %X**->\r\n\r\n", modbus_rx.error);
   }   
}

void unknown_func()
{
   //DEBUG_MSG("Trying unknown function\r\n");
   //DEBUG_MSG("Diagnostic:\r\n");
   if(!(modbus_diagnostics(MODBUS_SLAVE_ADDRESS,0,0)))
   {
      //DEBUG_MSG("Data:");
      //for(i=0; i < (modbus_rx.len); ++i)
      //   DEBUG_DATA("%X ", modbus_rx.data[i]);
      //DEBUG_MSG("\r\n\r\n");
   }
   else
   {
      //DEBUG_DATA("<-**Exception %X**->\r\n\r\n", modbus_rx.error);
   }
}


void main()
{
   fState = 0;
   for (posicion=0;posicion<0x10;posicion++){
      buffer[posicion] = 0x00;
      txbuf[posicion] = 0x00;
      rxbuf[posicion] = 0x00;
   }
   enable_interrupts(INT_SSP);
   modbus_init();   
   setup_wdt(WDT_ON);
   setup_timer_2(T2_DIV_BY_16, 255, 16); 
   setup_ccp1(CCP_PWM);
   setup_ccp2(CCP_PWM);
   set_pwm1_duty (0);
   set_pwm2_duty (0);
   enable_interrupts(global);
   //DEBUG_MSG("\r\nInitializing...");
   //DEBUG_MSG("...ready\r\n");
   Lee_Vector();
   

while(true)
{
   restart_wdt();
   txbuf[0]=10;
   txbuf[19]=128;
   //temporal++;   
   set_pwm1_duty(pwm1);
   set_pwm2_duty(pwm2);
   if(crc1==txbuf[0] && crc2==txbuf[19]){
      if(instruccion==3){
         read_all_holding(0,8);
      }
      if(instruccion==5){
         write_reg(address,reg);
         /*
         if(address_ant!=address){
            address_ant=address;
            if(reg_ant!=reg){
               reg_ant=reg;
               write_reg(address,reg);
            }         
         }
         */
      }  
   }
   //DEBUG_DATA("Inst: %02u \r\n", instruccion);
   //DEBUG_DATA("Address: %02u \r\n", address);
   Lee_Vector();   
}
}
