//#include <16F877A.h>
//#fuses HS
//#use delay(clock=20Mhz)
#include <18F4550.h>
#device adc=10
#fuses HSPLL,NOWDT,NOPROTECT,NOLVP,NODEBUG,USBDIV,PLL1,CPUDIV1,NOVREGEN,NOMCLR,NOPUT// PLL1 para 4 MHz
//#fuses XT,NOWDT,NOPROTECT,NOLVP,NODEBUG,VREGEN,NOMCLR// PLL1 para 4 MHz
#use delay(clock=48000000)
#include <LCD420.c>

int1 BITS[32];
int16 Ta,Tb,TICK;
int8 n; 
Int1 BIT_START, NUEVO_DATO;
int BYTE_IR[4];
int8 BIT_BYTE(int1 D7, int1 D6, int1 D5, int1 D4, int1 D3, int1 D2, int1 D1, int1 D0);
#int_EXT
void RB0(){
   Ta=get_timer1();
   TICK = Ta-tb;
   Tb = Ta;
  
   if(BIT_START==1){
       //BIT 1
       if(TICK>1350 && TICK < 1450)  // 1406.25
      {
         BITS[n]=1; n++;
      }
      //BIT 0
       if(TICK>650 && TICK < 750)  // TICK=700
      {
         BITS[n]=0; n++;
      }
      if(n==32){
         NUEVO_DATO=1;
      }
   
   }
   //BIT STAR
   if(TICK>8200 && TICK < 8600)  //8437  
   {
      BIT_START=1;
      n=0; 
   }
}
void main(){
   lcd_init();
   SETUP_TIMER_1(T1_INTERNAL|T1_DIV_BY_8);
   EXT_INT_EDGE(0,H_TO_L); // 
   enable_interrupts(INT_EXT);
   enable_interrupts(GLOBAL); 
   lcd_gotoxy(1,1);
   printf(lcd_putc,"       CABINA       ");
   lcd_gotoxy(1,2);
   printf(lcd_putc,"        PCR         ");
   lcd_gotoxy(1,3);
   printf(lcd_putc,"    JP INGLOBAL     ");
   lcd_gotoxy(1,4);
   printf(lcd_putc,"   REF:  JPCR-60    ");
   delay_ms(2000);
   printf(lcd_putc,"\f");  
   
   while(TRUE){
      
      if(NUEVO_DATO==1){
         NUEVO_DATO=0;
         BYTE_IR[0]=BIT_BYTE(BITS[0],BITS[1],BITS[2],BITS[3],BITS[4],BITS[5],BITS[6],BITS[7]);
         BYTE_IR[1]=BIT_BYTE(BITS[8],BITS[9],BITS[10],BITS[11],BITS[12],BITS[13],BITS[14],BITS[15]);
         BYTE_IR[2]=BIT_BYTE(BITS[16],BITS[17],BITS[18],BITS[19],BITS[20],BITS[21],BITS[22],BITS[23]);
         BYTE_IR[3]=BIT_BYTE(BITS[24],BITS[25],BITS[26],BITS[27],BITS[28],BITS[29],BITS[30],BITS[31]);
         lcd_gotoxy(1,1);
         printf(lcd_putc,"B0=%X   B1=%X",BYTE_IR[0],BYTE_IR[1]);
         lcd_gotoxy(1,2);
         printf(lcd_putc,"B2=%X   B3=%X",BYTE_IR[2],BYTE_IR[3]);    
         if(BYTE_IR[2]==0x45) output_d(0xFF);
         if(BYTE_IR[2]==0x46) output_d(0x00); 
      }
      lcd_gotoxy(1,3);
   printf(lcd_putc,"    TICK: %04Lu  ",TICK);
   }
   
}
int8 BIT_BYTE(int1 D0, int1 D1, int1 D2, int1 D3, int1 D4, int1 D5, int1 D6, int1 D7){
   int8 dato;
   dato= D7*128 + D6*64 + D5*32 + D4*16 + D3*8 + D2*4 + D1*2 +D0*1; // 0 al 255 
   return dato; 
}
