//For LCD RX - PA7,TX - PB2

#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>

#define output(directions,pin) (directions |= pin) // set port direction for output
#define input(directions,pin) (directions &= (~pin)) // set port direction for
#define set(port,pin) (port |= pin) // set port pin
#define clear(port,pin) (port &= (~pin)) // clear port pin
#define pin_test(pins,pin) (pins & pin) // test for port pin
#define bit_test(byte,bit) (byte & (1 << bit)) // test for bit set

// For Serial Communicaiton
#define bit_delay_time 8.5 // bit delay for 115200 with overhead
#define bit_delay() _delay_us(bit_delay_time) // RS232 bit delay
#define half_bit_delay() _delay_us(bit_delay_time/2) // RS232 half bit delay
#define char_delay() _delay_ms(10) // char delay
#define max_buffer 25

//For LED
#define led (1 << PA6)
#define led_port PORTA
#define led_direction DDRA


// For LCD
#define LCD_port PORTA
#define LCD_direction DDRA
#define DB7 (1 << PA0)
#define DB6 (1 << PA1)
#define DB5 (1 << PA2)
#define DB4 (1 << PA3)
#define E (1 << PA4)
#define RS (1 << PA5)

#define long_delay() _delay_ms(1000) // delay before redraw
#define lcd_delay() _delay_ms(10) // delay between commands
#define strobe_delay() _delay_us(1) // delay for strobe

//
// lcd_putchar
//    put character in lcdbyte
//
void lcd_putchar(char lcdbyte) {
   //
   // set RS for data
   //
   set(LCD_port, RS);
   //
   // output high nibble
   //
   if bit_test(lcdbyte, 7)
      set(LCD_port, DB7);
   else
      clear(LCD_port, DB7);
   if bit_test(lcdbyte, 6)
      set(LCD_port, DB6);
   else
      clear(LCD_port, DB6);
   if bit_test(lcdbyte, 5)
      set(LCD_port, DB5);
   else
      clear(LCD_port, DB5);
   if bit_test(lcdbyte, 4)
      set(LCD_port, DB4);
   else
      clear(LCD_port, DB4);
   //
   // strobe E
   //
   strobe_delay();
   set(LCD_port, E);
   strobe_delay();
   clear(LCD_port, E);
   //
   // wait
   //
   lcd_delay();
   //
   // output low nibble
   //
   if bit_test(lcdbyte, 3)
      set(LCD_port, DB7);
   else
      clear(LCD_port, DB7);
   if bit_test(lcdbyte, 2)
      set(LCD_port, DB6);
   else
      clear(LCD_port, DB6);
   if bit_test(lcdbyte, 1)
      set(LCD_port, DB5);
   else
      clear(LCD_port, DB5);
   if bit_test(lcdbyte, 0)
      set(LCD_port, DB4);
   else
      clear(LCD_port, DB4);
   //
   // strobe E
   //
   strobe_delay();
   set(LCD_port, E);
   strobe_delay();
   clear(LCD_port, E);
   //
   // wait and return
   //
   lcd_delay();
   }
//
// lcd_putcmd
//    put command in lcdbyte
//
void lcd_putcmd(char lcdbyte) {
   //
   // clear RS for command
   //
   clear(LCD_port, RS);
   //
   // output command bits
   //
   PORTA = lcdbyte;
   //
   // strobe E
   //
   strobe_delay();
   set(LCD_port, E);
   strobe_delay();
   clear(LCD_port, E);
   //
   // wait and return
   //
   lcd_delay();
   }
//
// lcd_putstring
//    put a null-terminated string in flash
//
void lcd_putstring(PGM_P message) {
   static uint8_t index;
   static char chr;
   index = 0;
   while (1) {
      chr = pgm_read_byte(&(message[index]));
      if (chr == 0)
         return;
      lcd_putchar(chr);
      ++index;
      }
   }
//
// lcd_init
//    initialize the LCD
//
void lcd_init() {
   //
   // power-up delay
   //
   lcd_delay();
   //
   // initialization sequence
   //
   lcd_putcmd(DB5+DB4);
   lcd_putcmd(DB5+DB4);
   lcd_putcmd(DB5+DB4);
   //
   // 4-bit interface
   //
   lcd_putcmd(DB5);
   //
   // two lines, 5x7 font
   //
   lcd_putcmd(DB5);
   lcd_putcmd(DB7);
   //
   // display on
   //
   lcd_putcmd(0);
   lcd_putcmd(DB7+DB6+DB5);
   //
   // entry mode
   //
   lcd_putcmd(0);
   lcd_putcmd(DB6+DB5);
   }

void get_char(volatile unsigned char *pins, unsigned char pin, char *rxbyte) {
   //
   // read character into rxbyte on pins pin
   //    assumes line driver (inverts bits)
   //
   *rxbyte = 0;
   while (pin_test(*pins,pin))
      //
      // wait for start bit
      //
      ;
   //
   // delay to middle of first data bit
   //
   half_bit_delay();
   bit_delay();
   //
   // unrolled loop to read data bits
   //
   if pin_test(*pins,pin)
      *rxbyte |= (1 << 0);
   else
      *rxbyte |= (0 << 0);
   bit_delay();
   if pin_test(*pins,pin)
      *rxbyte |= (1 << 1);
   else
      *rxbyte |= (0 << 1);
   bit_delay();
   if pin_test(*pins,pin)
      *rxbyte |= (1 << 2);
   else
      *rxbyte |= (0 << 2);
   bit_delay();
   if pin_test(*pins,pin)
      *rxbyte |= (1 << 3);
   else
      *rxbyte |= (0 << 3);
   bit_delay();
   if pin_test(*pins,pin)
      *rxbyte |= (1 << 4);
   else
      *rxbyte |= (0 << 4);
   bit_delay();
   if pin_test(*pins,pin)
      *rxbyte |= (1 << 5);
   else
      *rxbyte |= (0 << 5);
   bit_delay();
   if pin_test(*pins,pin)
      *rxbyte |= (1 << 6);
   else
      *rxbyte |= (0 << 6);
   bit_delay();
   if pin_test(*pins,pin)
      *rxbyte |= (1 << 7);
   else
      *rxbyte |= (0 << 7);
   //
   // wait for stop bit
   //
   bit_delay();
   half_bit_delay();
}


void flash() {
    output(DDRA,(1 << PA6));
    clear(PORTA, (1 << PA6));
    _delay_ms(50);
    set(PORTA, (1 << PA6));
    _delay_ms(50);
    clear(PORTA, (1 << PA6));
}

int main(void) {

    static char chr;

   // set clock divider to /1
   CLKPR = (1 << CLKPCE);
   CLKPR = (0 << CLKPS3) | (0 << CLKPS2) | (0 << CLKPS1) | (0 << CLKPS0);

   // initialize LCD pins
   clear(LCD_port, DB7);
   output(LCD_direction, DB7);
   clear(LCD_port, DB6);
   output(LCD_direction, DB6);
   clear(LCD_port, DB5);
   output(LCD_direction, DB5);
   clear(LCD_port, DB4);
   output(LCD_direction, DB4);
   clear(LCD_port, E);
   output(LCD_direction, E);
   clear(LCD_port, RS);
   output(LCD_direction, RS);

   // initialize LCD
   lcd_init();

////output(led_direction, led);


    while (1) {

      // go to zero position
      lcd_putcmd(0);
      lcd_putcmd(DB5);

      // print first line from flash
      static const char line1[] PROGMEM = "Char from Serial";
      lcd_putstring((PGM_P) line1);

      // move to second line
      lcd_putcmd(DB7+DB6);
      lcd_putcmd(0);

      // get chhar from serial
        get_char(&PINA, (1 << PA7), &chr);

      // print char from serial
      lcd_putchar(chr);

      // pause
      long_delay();

      // clear display
      lcd_putcmd(0);
      lcd_putcmd(DB4);

      }
}


