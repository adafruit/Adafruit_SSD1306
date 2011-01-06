#define OLED_DC 11
#define OLED_CS 12
#define OLED_CLK 10
#define OLED_MOSI 9
#define OLED_RESET 13

#include <SSD1306.h>

SSD1306 oled(OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2


#define LOGO16_GLCD_HEIGHT 16 
#define LOGO16_GLCD_WIDTH  16 
static unsigned char __attribute__ ((progmem)) logo16_glcd_bmp[]={
0x30, 0xf0, 0xf0, 0xf0, 0xf0, 0x30, 0xf8, 0xbe, 0x9f, 0xff, 0xf8, 0xc0, 0xc0, 0xc0, 0x80, 0x00, 
0x20, 0x3c, 0x3f, 0x3f, 0x1f, 0x19, 0x1f, 0x7b, 0xfb, 0xfe, 0xfe, 0x07, 0x07, 0x07, 0x03, 0x00, };


void setup()   {                
  Serial.begin(9600);
  
  // If you want to provide external 7-9V VCC, uncomment next line and comment the one after
  //oled.ssd1306_init(SSD1306_EXTERNALVCC);
  
  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  oled.ssd1306_init(SSD1306_SWITCHCAPVCC);

  // init done
  
  // this is the test command, turns on all the pixels
  //SSD1306_command(SSD1306_DISPLAYALLON); 
  // then you can clear the display
  //oled.clear_display();
  
  oled.display(); // show splashscreen
  delay(2000);
  oled.clear();   // clears the screen and buffer

  // draw a single pixel
  oled.setpixel(10, 10, WHITE);
  oled.display();
  delay(2000);
  oled.clear();

  // draw many lines
  testdrawline();
  oled.display();
  delay(2000);
  oled.clear();

  // draw rectangles
  testdrawrect();
  oled.display();
  delay(2000);
  oled.clear();

  // draw multiple rectangles
  testfillrect();
  oled.display();
  delay(2000);
  oled.clear();

  // draw mulitple circles
  testdrawcircle();
  oled.display();
  delay(2000);
  oled.clear();

  // draw a white circle, 10 pixel radius, at location (32,32)
  oled.fillcircle(32, 32, 10, WHITE);
  oled.display();
  delay(2000);
  oled.clear();

  // draw the first ~12 characters in the font
  testdrawchar();
  oled.display();
  delay(2000);
  oled.clear();

  // draw a string at location (0,0)
  oled.drawstring(0, 0, "Lorem ipsum dolor sit amet, consectetur adipisicing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation");
  oled.display();
  delay(2000);
  oled.clear();

  // miniature bitmap display
  oled.drawbitmap(30, 30,  logo16_glcd_bmp, 16, 16, 1);
  oled.display();

  // invert the display
  oled.ssd1306_command(SSD1306_INVERTDISPLAY);
  delay(1000); 
  oled.ssd1306_command(SSD1306_NORMALDISPLAY);
  delay(1000); 

  // draw a bitmap icon and 'animate' movement
  testdrawbitmap(logo16_glcd_bmp, LOGO16_GLCD_HEIGHT, LOGO16_GLCD_WIDTH);
}


void loop()                     
{
  for (uint8_t i=0; i<128; i++) {
    for (uint8_t j=0; j<64; j++) {
      oled.setpixel(i, j, WHITE);
      oled.display();
    }
  }
  

}


void testdrawbitmap(const uint8_t *bitmap, uint8_t w, uint8_t h) {
  uint8_t icons[NUMFLAKES][3];
  srandom(666);     // whatever seed
 
  // initialize
  for (uint8_t f=0; f< NUMFLAKES; f++) {
    icons[f][XPOS] = random() % 128;
    icons[f][YPOS] = 0;
    icons[f][DELTAY] = random() % 5 + 1;
    
    Serial.print("x: ");
    Serial.print(icons[f][XPOS], DEC);
    Serial.print(" y: ");
    Serial.print(icons[f][YPOS], DEC);
    Serial.print(" dy: ");
    Serial.println(icons[f][DELTAY], DEC);
  }

  while (1) {
    // draw each icon
    for (uint8_t f=0; f< NUMFLAKES; f++) {
      oled.drawbitmap(icons[f][XPOS], icons[f][YPOS], logo16_glcd_bmp, w, h, WHITE);
    }
    oled.display();
    delay(200);
    
    // then erase it + move it
    for (uint8_t f=0; f< NUMFLAKES; f++) {
      oled.drawbitmap(icons[f][XPOS], icons[f][YPOS],  logo16_glcd_bmp, w, h, BLACK);
      // move it
      icons[f][YPOS] += icons[f][DELTAY];
      // if its gone, reinit
      if (icons[f][YPOS] > 64) {
	icons[f][XPOS] = random() % 128;
	icons[f][YPOS] = 0;
	icons[f][DELTAY] = random() % 5 + 1;
      }
    }
  }
}


void testdrawchar(void) {
  for (uint8_t i=0; i < 168; i++) {
    oled.drawchar((i % 21) * 6, i/21, i);
  }    
}

void testdrawcircle(void) {
  for (uint8_t i=0; i<64; i+=2) {
    oled.drawcircle(63, 31, i, WHITE);
  }
}

void testdrawrect(void) {
  for (uint8_t i=0; i<64; i+=2) {
    oled.drawrect(i, i, 128-i, 64-i, WHITE);
  }
}

void testfillrect(void) {
  for (uint8_t i=0; i<64; i++) {
      // alternate colors for moire effect
    oled.fillrect(i, i, 128-i, 64-i, i%2);
  }
}

void testdrawline() {
  for (uint8_t i=0; i<128; i+=4) {
    oled.drawline(0, 0, i, 63, WHITE);
    oled.display();
  }
  for (uint8_t i=0; i<64; i+=4) {
    oled.drawline(0, 0, 127, i, WHITE);
    oled.display();
  }

  delay(1000);

  for (uint8_t i=0; i<128; i+=4) {
    oled.drawline(i, 63, 0, 0, BLACK);
    oled.display();
  }
  for (uint8_t i=0; i<64; i+=4) {
    oled.drawline(127, i, 0, 0, BLACK);
    oled.display();
  }
}
