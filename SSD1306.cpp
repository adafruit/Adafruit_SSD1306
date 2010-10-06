// some of this code was written by <cstone@pobox.com> originally; 
// it is in the public domain.

//#include <Wire.h>
#include <avr/pgmspace.h>
#include <WProgram.h>
#include <util/delay.h>
#include <stdlib.h>

#include "SSD1306.h"

static uint8_t is_reversed = 0;

// a handy reference to where the pages are on the screen
const uint8_t pagemap[] = { 0, 1, 2, 3, 4, 5, 6, 7};

// a 5x7 font table
extern uint8_t PROGMEM font[];

// the memory buffer for the LCD
static uint8_t buffer[1024] = { 
0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 
0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 
0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 
0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 
0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 
0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 
0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 
0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 

0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 
0X0, 0X0, 0X0, 0XC0, 0XE0, 0XF0, 0XF8, 0XF8, 0XCF, 0XCF, 0XCF, 0XCF, 0XE0, 0X0, 0X0, 0X0, 
0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 
0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 
0X0, 0X0, 0XFE, 0XFC, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 
0X0, 0X0, 0XF8, 0XFC, 0XE, 0XE, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 
0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X60, 0X60, 0X0, 0X0, 0X0, 0XC0, 0XC0, 
0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 

0XF8, 0XF8, 0XF8, 0XF8, 0XF8, 0XF8, 0XF8, 0XF8, 0XF8, 0XF8, 0XF8, 0XF8, 0XF8, 0XF0, 0XE0, 0XE0, 
0XE0, 0XFC, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0X7F, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0X7C, 0X0, 0X0, 
0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0XF0, 0XFC, 
0XE, 0X6, 0X6, 0X6, 0X6, 0XC, 0XFE, 0XFC, 0X0, 0X0, 0XF8, 0XFC, 0XE, 0X6, 0X6, 0X6, 
0X6, 0X9C, 0XFF, 0XFF, 0X0, 0X60, 0XF8, 0X9C, 0X6, 0X6, 0X6, 0X6, 0XC, 0XFC, 0XFE, 0X0, 
0X0, 0X6, 0XFF, 0XFF, 0X6, 0X6, 0X0, 0XFE, 0XFE, 0XE, 0X6, 0X6, 0X2, 0X0, 0XFE, 0XFE, 
0X0, 0X0, 0X0, 0X0, 0XFE, 0XFE, 0X0, 0X0, 0X0, 0XFE, 0XFE, 0X0, 0X0, 0X6, 0XFF, 0XFF, 
0X6, 0X6, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 

0X1, 0X1F, 0X3F, 0X7F, 0X7F, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XF7, 0XE7, 0XE7, 0XC7, 
0XCF, 0X9F, 0XFF, 0XFF, 0XFF, 0XEF, 0XE0, 0XF8, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XE0F, 0XFF, 
0XFE, 0XFE, 0XFE, 0XFE, 0XFE, 0XFE, 0XFC, 0XFC, 0XF8, 0XF0, 0XE0, 0XC0, 0X0, 0X0, 0X0, 0X3, 
0X7, 0X6, 0X4, 0X4, 0X6, 0X7, 0X7, 0X7, 0X0, 0X0, 0X1, 0X3, 0X7, 0X6, 0X4, 0X6, 
0X6, 0X7, 0X7, 0X7, 0X0, 0X0, 0X1, 0X3, 0X6, 0X6, 0X4, 0X6, 0X6, 0X7, 0X7, 0X0, 
0X0, 0X0, 0X7, 0X7, 0X0, 0X0, 0X0, 0X7, 0X7, 0X0, 0X0, 0X0, 0X0, 0X0, 0X1, 0X7, 
0X6, 0X6, 0X6, 0X6, 0X7, 0X1, 0X0, 0X0, 0X0, 0X7, 0X7, 0X0, 0X0, 0X0, 0X7, 0X7, 
0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 

0X0, 0X0, 0X0, 0XC0, 0XE0, 0XF8, 0XF9, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XBF, 0X8F, 0XC7, 
0XC7, 0XF3, 0XFF, 0XFF, 0XFF, 0XFF, 0XF, 0X3F, 0XFE, 0XFC, 0XFC, 0XFC, 0XFC, 0XFE, 0XFF, 0XFF, 
0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0X7F, 0X3F, 0XF, 0X7, 0X1, 0X0, 0X0, 0X0, 0X30, 
0X38, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 
0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0XFE, 0XFE, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 
0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0XE0, 0XE0, 0X0, 
0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X38, 0X30, 0X0, 0X0, 0X0, 0X0, 0X0, 
0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 

0X0, 0XE0, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0X7F, 0X7F, 0X7F, 0X7F, 0X3F, 0X1F, 
0X1F, 0XF, 0X7F, 0XFF, 0XFF, 0XFF, 0XFE, 0XFC, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XF8, 
0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0XFF, 
0XFF, 0X0, 0X0, 0X0, 0XFF, 0XFF, 0X7, 0X3, 0X3, 0X3, 0XFF, 0XFE, 0X0, 0X0, 0X78, 0XFE, 
0X87, 0X3, 0X3, 0X3, 0X3, 0X86, 0XFF, 0XFF, 0X0, 0X0, 0X7F, 0XFF, 0X80, 0X0, 0X0, 0X0, 
0XFF, 0XFF, 0X0, 0X0, 0X84, 0X9F, 0X1F, 0X3B, 0X33, 0XF3, 0XE0, 0X0, 0X3, 0XFF, 0XFF, 0X3, 
0X1, 0X0, 0XFF, 0XFF, 0X3, 0X3, 0X1, 0X0, 0X0, 0XFF, 0XFF, 0X0, 0X0, 0XF8, 0XFE, 0X9F, 
0X13, 0X13, 0X13, 0X13, 0X9E, 0X9C, 0X0, 0X0, 0X8E, 0X9F, 0X1B, 0X33, 0X73, 0XE2, 0XC0, 0X0, 

0X0, 0X0, 0X0, 0X0, 0X1, 0X1, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 
0X0, 0X0, 0X0, 0X1, 0X3, 0X7, 0XF, 0X1F, 0X1F, 0X3F, 0X3F, 0X3F, 0X3F, 0X1F, 0XF, 0X3, 
0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X3, 
0X3, 0X0, 0X0, 0X0, 0X3, 0X3, 0X0, 0X0, 0X0, 0X0, 0X3, 0X3, 0X0, 0X0, 0X0, 0X1, 
0X3, 0X3, 0X3, 0X3, 0X3, 0X1, 0X3, 0X3, 0X0, 0X0, 0X0, 0X1, 0X3, 0X3, 0X3, 0X3, 
0X3, 0X1, 0X0, 0X0, 0X1, 0X3, 0X3, 0X3, 0X3, 0X3, 0X0, 0X0, 0X0, 0X3, 0X3, 0X0, 
0X0, 0X0, 0X3, 0X1, 0X0, 0X0, 0X0, 0X0, 0X0, 0X3, 0X3, 0X0, 0X0, 0X0, 0X1, 0X3, 
0X3, 0X3, 0X3, 0X3, 0X1, 0X1, 0X0, 0X0, 0X1, 0X3, 0X3, 0X3, 0X3, 0X1, 0X0, 0X0, 

0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0,
0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0,
0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0,
0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0,
0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0,
0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0,
0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0,
0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0,


};



void SSD1306::drawbitmap(uint8_t x, uint8_t y, 
			const uint8_t *bitmap, uint8_t w, uint8_t h,
			uint8_t color) {
  for (uint8_t j=0; j<h; j++) {
    for (uint8_t i=0; i<w; i++ ) {
      if (pgm_read_byte(bitmap + i + (j/8)*w) & _BV(j%8)) {
	setpixel(x+i, y+j, color);
      }
    }
  }
}

void SSD1306::drawstring(uint8_t x, uint8_t line, char *c) {
  while (c[0] != 0) {
    drawchar(x, line, c[0]);
    c++;
    x += 6; // 6 pixels wide
    if (x + 6 >= SSD1306_LCDWIDTH) {
      x = 0;    // ran out of this line
      line++;
    }
    if (line >= (SSD1306_LCDHEIGHT/8))
      return;        // ran out of space :(
  }

}

void  SSD1306::drawchar(uint8_t x, uint8_t line, char c) {
  for (uint8_t i =0; i<5; i++ ) {
    buffer[x + (line*128) ] = pgm_read_byte(font+(c*5)+i);
    x++;
  }
}


// bresenham's algorithm - thx wikpedia
void SSD1306::drawline(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, 
		      uint8_t color) {
  uint8_t steep = abs(y1 - y0) > abs(x1 - x0);
  if (steep) {
    swap(x0, y0);
    swap(x1, y1);
  }

  if (x0 > x1) {
    swap(x0, x1);
    swap(y0, y1);
  }

  uint8_t dx, dy;
  dx = x1 - x0;
  dy = abs(y1 - y0);

  int8_t err = dx / 2;
  int8_t ystep;

  if (y0 < y1) {
    ystep = 1;
  } else {
    ystep = -1;}

  for (; x0<x1; x0++) {
    if (steep) {
      setpixel(y0, x0, color);
    } else {
      setpixel(x0, y0, color);
    }
    err -= dy;
    if (err < 0) {
      y0 += ystep;
      err += dx;
    }
  }
}

// filled rectangle
void SSD1306::fillrect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, 
		      uint8_t color) {

  // stupidest version - just pixels - but fast with internal buffer!
  for (uint8_t i=x; i<x+w; i++) {
    for (uint8_t j=y; j<y+h; j++) {
      setpixel(i, j, color);
    }
  }
}

// draw a rectangle
void SSD1306::drawrect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, 
		      uint8_t color) {
  // stupidest version - just pixels - but fast with internal buffer!
  for (uint8_t i=x; i<x+w; i++) {
    setpixel(i, y, color);
    setpixel(i, y+h-1, color);
  }
  for (uint8_t i=y; i<y+h; i++) {
    setpixel(x, i, color);
    setpixel(x+w-1, i, color);
  } 
}

// draw a circle outline
void SSD1306::drawcircle(uint8_t x0, uint8_t y0, uint8_t r, 
			uint8_t color) {
  int8_t f = 1 - r;
  int8_t ddF_x = 1;
  int8_t ddF_y = -2 * r;
  int8_t x = 0;
  int8_t y = r;

  setpixel(x0, y0+r, color);
  setpixel(x0, y0-r, color);
  setpixel(x0+r, y0, color);
  setpixel(x0-r, y0, color);

  while (x<y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;
  
    setpixel(x0 + x, y0 + y, color);
    setpixel(x0 - x, y0 + y, color);
    setpixel(x0 + x, y0 - y, color);
    setpixel(x0 - x, y0 - y, color);
    
    setpixel(x0 + y, y0 + x, color);
    setpixel(x0 - y, y0 + x, color);
    setpixel(x0 + y, y0 - x, color);
    setpixel(x0 - y, y0 - x, color);
    
  }
}

void SSD1306::fillcircle(uint8_t x0, uint8_t y0, uint8_t r, 
			uint8_t color) {
  int8_t f = 1 - r;
  int8_t ddF_x = 1;
  int8_t ddF_y = -2 * r;
  int8_t x = 0;
  int8_t y = r;

  for (uint8_t i=y0-r; i<=y0+r; i++) {
    setpixel(x0, i, color);
  }

  while (x<y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;
  
    for (uint8_t i=y0-y; i<=y0+y; i++) {
      setpixel(x0+x, i, color);
      setpixel(x0-x, i, color);
    } 
    for (uint8_t i=y0-x; i<=y0+x; i++) {
      setpixel(x0+y, i, color);
      setpixel(x0-y, i, color);
    }    
  }
}

// the most basic function, set a single pixel
void SSD1306::setpixel(uint8_t x, uint8_t y, uint8_t color) {
  if ((x >= SSD1306_LCDWIDTH) || (y >= SSD1306_LCDHEIGHT))
    return;

  // x is which column
  if (color == WHITE) 
    buffer[x+ (y/8)*128] |= _BV((y%8));  
  else
    buffer[x+ (y/8)*128] &= ~_BV((y%8)); 
}


void SSD1306::ssd1306_init(uint8_t vccstate) {
  // set pin directions
  pinMode(sid, OUTPUT);
  pinMode(sclk, OUTPUT);
  pinMode(dc, OUTPUT);
  pinMode(rst, OUTPUT);
  pinMode(cs, OUTPUT);


  digitalWrite(rst, HIGH);
  // VDD (3.3V) goes high at start, lets just chill for a ms
  delay(1);
  // bring0xset low
  digitalWrite(rst, LOW);
  // wait 10ms
  delay(10);
  // bring out of reset
  digitalWrite(rst, HIGH);
  // turn on VCC (9V?)

  ssd1306_command(SSD1306_DISPLAYOFF);  // 0xAE
  ssd1306_command(SSD1306_SETLOWCOLUMN | 0x0);  // low col = 0
  ssd1306_command(SSD1306_SETHIGHCOLUMN | 0x0);  // hi col = 0
  
  ssd1306_command(SSD1306_SETSTARTLINE | 0x0); // line #0

  ssd1306_command(SSD1306_SETCONTRAST);  // 0x81
  if (vccstate == SSD1306_EXTERNALVCC) {
    ssd1306_command(0x9F);  // external 9V
  } else {
    ssd1306_command(0xCF);  // chargepump
  }
    
  ssd1306_command(0xa1);  // setment remap 95 to 0 (?)

  ssd1306_command(SSD1306_NORMALDISPLAY); // 0xA6

  ssd1306_command(SSD1306_DISPLAYALLON_RESUME); // 0xA4

  ssd1306_command(SSD1306_SETMULTIPLEX); // 0xA8
  ssd1306_command(0x3F);  // 0x3F 1/64 duty
  
  ssd1306_command(SSD1306_SETDISPLAYOFFSET); // 0xD3
  ssd1306_command(0x0); // no offset
  
  ssd1306_command(SSD1306_SETDISPLAYCLOCKDIV);  // 0xD5
  ssd1306_command(0x80);  // the suggested ratio 0x80
  
  ssd1306_command(SSD1306_SETPRECHARGE); // 0xd9
  if (vccstate == SSD1306_EXTERNALVCC) {
    ssd1306_command(0x22); // external 9V
  } else {
    ssd1306_command(0xF1); // DC/DC
  }
  
  ssd1306_command(SSD1306_SETCOMPINS); // 0xDA
  ssd1306_command(0x12); // disable COM left/right remap
  
  ssd1306_command(SSD1306_SETVCOMDETECT); // 0xDB
  ssd1306_command(0x40); // 0x20 is default?

  ssd1306_command(SSD1306_MEMORYMODE); // 0x20
  ssd1306_command(0x00); // 0x0 act like ks0108
  
  // left to right scan
  ssd1306_command(SSD1306_SEGREMAP | 0x1);

  ssd1306_command(SSD1306_COMSCANDEC);

  ssd1306_command(SSD1306_CHARGEPUMP); //0x8D
  if (vccstate == SSD1306_EXTERNALVCC) {
    ssd1306_command(0x10);  // disable
  } else {
    ssd1306_command(0x14);  // disable    
  }
  ssd1306_command(SSD1306_DISPLAYON);//--turn on oled panel
}


void SSD1306::invert(uint8_t i) {
  if (i) {
    ssd1306_command(SSD1306_INVERTDISPLAY);
  } else {
    ssd1306_command(SSD1306_NORMALDISPLAY);
  }
}

inline void SSD1306::spiwrite(uint8_t c) {
  shiftOut(sid, sclk, MSBFIRST, c);

}
void SSD1306::ssd1306_command(uint8_t c) { 
  digitalWrite(cs, HIGH);
  digitalWrite(dc, LOW);
  digitalWrite(cs, LOW);
  spiwrite(c);
  digitalWrite(cs, HIGH);
}

void SSD1306::ssd1306_data(uint8_t c) {
  digitalWrite(cs, HIGH);
  digitalWrite(dc, HIGH);
  digitalWrite(cs, LOW);
  spiwrite(c);
  digitalWrite(cs, HIGH);
}

void SSD1306::ssd1306_set_brightness(uint8_t val) {
  
}


void SSD1306::display(void) {
  ssd1306_command(SSD1306_SETLOWCOLUMN | 0x0);  // low col = 0
  ssd1306_command(SSD1306_SETHIGHCOLUMN | 0x0);  // hi col = 0
  ssd1306_command(SSD1306_SETSTARTLINE | 0x0); // line #0

  for (uint16_t i=0; i<1024; i++) {
    ssd1306_data(buffer[i]);
  }

}

// clear everything
void SSD1306::clear(void) {
  memset(buffer, 0, 1024);
}

void SSD1306::clear_display(void) {
 
}
