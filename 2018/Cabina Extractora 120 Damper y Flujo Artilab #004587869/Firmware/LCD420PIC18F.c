////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//           LIBRERIA PARA CONTROL DE LCD 4X20 con PIC 18F4550                //
//                                                                            //
//                                                                            //
//      Se debe llamar a este archivo mediante #include <LCD420PIC18_RyP.c>   //
//                                                                            //
//      Es necesariodefinir el puerto a utilizar para el teclado mediante:    //
//      #define use_portb_lcd TRUE   //para el puerto B                       //
//      #define use_portd_lcd TRUE   //para el puerto D                       //
//                                                                            //
//      FUNCIONES:                                                            //
//      void lcd_init(void);         //Inicializa el lcd.                     //
//                                                                            //
//                                                                            //
//                            RobotyPic 2013                                  //
//                    http://robotypic.blogspot.com/                          //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

//Elecci?n del puerto y pines

#ifdef use_portd_lcd

#define LCD_DB4  PIN_D4
#define LCD_DB5  PIN_D5
#define LCD_DB6  PIN_D6
#define LCD_DB7  PIN_D7
#define LCD_E    PIN_D2
#define LCD_RS   PIN_D0
#define LCD_RW   PIN_D1

#else

#define LCD_DB4  PIN_B4
#define LCD_DB5  PIN_B5
#define LCD_DB6  PIN_B6
#define LCD_DB7  PIN_B7
#define LCD_E    PIN_B2
#define LCD_RS   PIN_B0
#define LCD_RW   PIN_B1

#endif

// If you only want a 6-pin interface to your LCD, then
// connect the R/W pin on the LCD to ground, and comment
// out the following line.

#define USE_LCD_RW  1    

// If you use the PLL of Microcontroller to increase the
// clock to 48MHz use this define, else comment out the
// following line.

#define lcd_type 2
#define lcd_line_two 0x40

BYTE lcdline;

int8 const LCD_INIT_STRING[4] =
{
 0x20 | (lcd_type << 2), // Func set: 4-bit, 2 lines, 5x8 dots
 0xc,                    // Display on
 1,                      // Clear display
 6                      // Increment cursor
 };


//-------------------------------------
void lcd_send_nibble(int8 nibble)
{
 output_bit(LCD_DB4, !!(nibble & 1));
 output_bit(LCD_DB5, !!(nibble & 2)); 
 output_bit(LCD_DB6, !!(nibble & 4));  
 output_bit(LCD_DB7, !!(nibble & 8));  

 #ifdef FAST
 delay_cycles(25);
 #else
 delay_cycles(1);
 #endif
 output_high(LCD_E);
 #ifdef FAST
 delay_us(50);
 #else
 delay_us(2);
 #endif
 output_low(LCD_E);
}

#ifdef USE_LCD_RW
int8 lcd_read_nibble(void)
{
int8 retval;
#bit retval_0 = retval.0
#bit retval_1 = retval.1
#bit retval_2 = retval.2
#bit retval_3 = retval.3

retval = 0;
  
output_high(LCD_E);
#ifdef FAST
delay_cycles(25);
#else
delay_cycles(1);
#endif

retval_0 = input(LCD_DB4);
retval_1 = input(LCD_DB5);
retval_2 = input(LCD_DB6);
retval_3 = input(LCD_DB7);
 
output_low(LCD_E);
  
return(retval);  
}  
#endif

#ifdef USE_LCD_RW
int8 lcd_read_byte(void)
{
int8 low;
int8 high;

output_high(LCD_RW);
#ifdef FAST
delay_cycles(25);
#else
delay_cycles(1);
#endif

high = lcd_read_nibble();

low = lcd_read_nibble();

return( (high<<4) | low);
}
#endif

void lcd_send_byte(int8 address, int8 n)
{
output_low(LCD_RS);

//#ifdef USE_LCD_RW
//while(bit_test(lcd_read_byte(),7)) ;
//#else
#ifdef FAST
delay_ms(2); 
#else
delay_us(60);
#endif
//#endif

if(address)
  output_high(LCD_RS);
else
  output_low(LCD_RS);
 
 #ifdef FAST
 delay_cycles(25);
 #else
 delay_cycles(1);
 #endif

#ifdef USE_LCD_RW
output_low(LCD_RW);
#ifdef FAST
 delay_cycles(25);
 #else
 delay_cycles(1);
 #endif
#endif

output_low(LCD_E);

lcd_send_nibble(n >> 4);
lcd_send_nibble(n & 0xf);
}

void lcd_init(void)
{
int8 i;

output_low(LCD_RS);

#ifdef USE_LCD_RW
output_low(LCD_RW);
#endif

output_low(LCD_E);

delay_ms(15);

for(i=0 ;i < 3; i++)
  {
    lcd_send_nibble(0x03);
    delay_ms(5);
  }

lcd_send_nibble(0x02);

for(i=0; i < sizeof(LCD_INIT_STRING); i++)
  {
    lcd_send_byte(0, LCD_INIT_STRING[i]);
  
    #ifndef USE_LCD_RW
    delay_ms(5);
    #endif
  }

}

void lcd_gotoxy( BYTE x, BYTE y)
{
  BYTE address;
 

  switch(y)
  {
    case 1 : address=0x80;
    lcdline=1;
    break;
    case 2 : address=0xc0;
    lcdline=2;
    break;
    case 3 : address=0x94;
    lcdline=3;
    break;
    case 4 : address=0xd4;
    lcdline=4;
    break;
  }

  address+=x-1;
  lcd_send_byte(0,address);
}


void lcd_putc(char c)
{
   if(c=='?')
    c=238;
 switch(c)
  {
    case '\f':
      lcd_send_byte(0,1);
      lcdline=1;
      delay_ms(2);
      break;
  
    case '\n':
      lcd_gotoxy(1,++lcdline);
      if (lcdline>4) lcd_gotoxy(1,1);
      break;
  
    case '\b':
      lcd_send_byte(0,0x10);
      break;
  
    default:
      lcd_send_byte(1,c);
      break;
  }
}

#ifdef USE_LCD_RW
char lcd_getc(int8 x, int8 y)
{
char value;

lcd_gotoxy(x,y);

// Wait until busy flag is low.
//while(bit_test(lcd_read_byte(),7)); 

output_high(LCD_RS);
value = lcd_read_byte();
output_low(lcd_RS);

return(value);
}
#endif
