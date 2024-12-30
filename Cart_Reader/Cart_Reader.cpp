#include "Cart_Reader.h"
/*
void serial_display::update() {
  delay(100); // Wait for UART to be flushed (is there a better way?)
}

void serial_display::clear() {
}

void serial_display::println(const String& string) {
  Serial.println(string);
}

void serial_display::println(byte message, int outputFormat) {
  Serial.println(message, outputFormat);
}

void serial_display::println(const char myString[]) {
  Serial.println(myString);
}

void serial_display::println(const __FlashStringHelper* string) {
  Serial.println(string);
}

void serial_display::println(long unsigned int message) {
  Serial.println(message);
}*/


/*screen_display::screen_display() {
  display.begin();
  display.setContrast(40);
  display.setFont(u8g2_font_haxrcorp4089_tr); // Font appears to be 8 rows x 4 cols
}*/

void screen_display::setup() {
  display.begin();
  display.setContrast(20);
  display.setFont(u8g2_font_haxrcorp4089_tr);
}

void screen_display::set_color(draw_color color) {
	display.setDrawColor(static_cast<uint8_t>(color));
}

void screen_display::update() {
  display.updateDisplay();
}

void screen_display::clear() {
  display.clearBuffer();
  display.setCursor(0, 8);
}

void screen_display::println(const String& string) {
  display.print(string);
  display.setCursor(0, display.ty + 8);
}

void screen_display::println(byte message, int outputFormat) {
  display.print(message, outputFormat);
  display.setCursor(0, display.ty + 8);
}

void screen_display::println(const char myString[]) {
  display.print(myString);
  display.setCursor(0, display.ty + 8);
}

void screen_display::println(const __FlashStringHelper* string) {
  print(string);
  display.setCursor(0, display.ty + 8);
}

void screen_display::println(long unsigned int message) {
  print(message);
  display.setCursor(0, display.ty + 8);
}

void screen_display::print(const String& string) {
  print(string);
}

void screen_display::print(byte message, int outputFormat) {
  display.print(message, outputFormat);
}

void screen_display::print(const char myString[]) {
  // test for word wrap
  if ((display.tx + strlen(myString) * 6) > 128) {
    unsigned int strPos = 0;
    // Print until end of display
    while ((display.tx < 122) && (myString[strPos] != '\0')) {
      display.print(myString[strPos]);
      strPos++;
    }
    // Newline
    display.setCursor(0, display.ty + 8);
    // Print until end of display and ignore remaining characters
    while ((strPos < strlen(myString)) && (display.tx < 122) && (myString[strPos] != '\0')) {
      display.print(myString[strPos]);
      strPos++;
    }
  } else {
    display.print(myString);
  }
}

void screen_display::print(const __FlashStringHelper* string) {
  display.print(string);
}

void screen_display::print(long unsigned int message) {
  display.print(message);
}

void screen_display::set_cursor(byte x, byte y) {
  display.setCursor(x, y);
}

byte screen_display::width() const {
  return 128;
}

byte screen_display::height() const {
  return 64;
}

