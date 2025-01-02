#ifndef CART_READER_H_
#define CART_READER_H_

#include <U8g2lib.h>
#include <RotaryEncoder.h>

enum class draw_color
{
	INVERTED,
	NORMAL,
	XOR,
};

struct rotary_data
{
  long position;
  int8_t direction;
};

static constexpr const uint8_t INPUT_BUTTON_NONE = 0x00;
static constexpr const uint8_t INPUT_BUTTON_PRESS = 0x01;
static constexpr const uint8_t INPUT_BUTTON_SHORT_RELEASE = 0x02;
static constexpr const uint8_t INPUT_BUTTON_LONG_RELEASE = 0x04;
static constexpr const uint8_t INPUT_ROTARY_POSITIVE = 0x08;
static constexpr const uint8_t INPUT_ROTARY_NEGATIVE = 0x10;

class screen_input {
public:
  screen_input(RotaryEncoder& encoder)
  :encoder(encoder), rotary(read_rotary()),
    previous_rotary(rotary), button(read_button()),
    previous_button(button), button_press_time(millis()), debounce_time(0)
  {}

  void tick()
  {
    previous_rotary = rotary;
    previous_button = button;
    encoder.tick();
    button = read_debounce_button();
    rotary = read_rotary();

    if (button && (previous_button != button)) {
      button_press_time = millis();
    }
  }

  bool get_button() const {
    return button;
  }

  rotary_data get_rotary() const {
    return rotary;
  }

  unsigned long get_button_press_time() const {
    return button_press_time;
  }

  uint8_t current_input_event() {
    uint8_t flags = 0;
    // Check if rotary encoder has changed
    if (rotary.position != previous_rotary.position) {
      if (rotary.direction == 1) {
        flags |= INPUT_ROTARY_POSITIVE;
      } else if (rotary.direction == -1) {
        flags |= INPUT_ROTARY_NEGATIVE;
      }
    }

    if (button == 0) {
      flags |= INPUT_BUTTON_PRESS;
    }
    if (button != previous_button) {
      if (button == 0) {
        button_press_time = millis();    
      } else {
        // Signal long press delay reached
        if ((millis() - button_press_time) > 2000) {
          flags |= INPUT_BUTTON_LONG_RELEASE;
        }
        // normal press
        else {
          flags |= INPUT_BUTTON_SHORT_RELEASE;
        }
      }
    }

    return flags;
  }

private:
  RotaryEncoder& encoder;
  rotary_data rotary;
  rotary_data previous_rotary;
  bool button;
  bool previous_button;
  unsigned long button_press_time;

  bool debounce_button;
  unsigned long debounce_time;

  bool read_debounce_button() {

    constexpr const unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers
    bool real_button = read_button();
    if (real_button != previous_button) {
      if (real_button != debounce_button) {
        debounce_time = millis();
        debounce_button = real_button;
      } else if ((millis() - debounce_time) > debounceDelay) {
        button = real_button;
      }
    }
    return button;
  }

  static bool read_button() {
    return (PING & (1 << PING2)) >> PING2;
  }

  rotary_data read_rotary() {
    return rotary_data {
      encoder.getPosition(),
      static_cast<int8_t>(encoder.getDirection())
    };
  }
};

class screen_display
{
public:
  screen_display(U8G2_ST7567_OS12864_F_4W_HW_SPI& disp)
  :display(disp)
  {}

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
