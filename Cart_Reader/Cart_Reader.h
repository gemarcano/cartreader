#ifndef CART_READER_H_
#define CART_READER_H_

#include <U8g2lib.h>

enum class draw_color
{
	INVERTED,
	NORMAL,
	XOR,
};

class screen_display
{
public:
  screen_display(U8G2_ST7567_OS12864_F_4W_HW_SPI& disp)
  :display(disp) {}
  void clear();
  void update();
  void setup();

  void println(const String& string);
  void println(byte message, int outputFormat);
  void println(const char myString[]);
  void println(const __FlashStringHelper* string);
  void println(long unsigned int message);

  void print(const String& string);
  void print(byte message, int outputFormat);
  void print(const char myString[]);
  void print(const __FlashStringHelper* string);
  void print(long unsigned int message);

  void set_color(draw_color color);
  void set_cursor(byte x, byte y);
  byte width() const;
  byte height() const;

  U8G2_ST7567_OS12864_F_4W_HW_SPI& get_display() {
    return display;
  }

private:
  // Resolution 128x64, bitmapped it appears.
  //U8G2_ST7567_OS12864_F_4W_HW_SPI display{U8G2_R2, /* cs=*/12, /* dc=*/11, /* reset=*/10};
  U8G2_ST7567_OS12864_F_4W_HW_SPI& display;
};

#endif//CART_READER_H_
