#ifndef MENU_H_
#define MENU_H_

#include "Cart_Reader.h"

#include <stdint.h>
#include <avr/io.h>
#include <U8g2lib.h>

template<class T>
class list_menu {
public:
  list_menu(screen_display& display, const __FlashStringHelper* question, const T answers[], uint8_t num_answers, uint8_t default_choice)
  :question(question), choice(default_choice), prompts(answers), num_prompts(num_answers), display(display)
  {
    draw();
  }

  void draw();
  void update(uint8_t choice);

  uint8_t get_choice() const {
    return choice;
  }
  uint8_t max_choices() const {
    return num_prompts;
  }
private:

  const __FlashStringHelper* question;
	uint8_t choice;
  const T* prompts;
	uint8_t num_prompts;

  screen_display& display;

  template<class T2>
  void print_loop(const T2* prompts, unsigned char start, unsigned char count)
  {
    for (unsigned char i = start; i < count; i++) {
      // Add space for the selection dot
      display.print("   ");
      // Print menu item
      display.println(prompts[i]);
    }
  }

  void print_loop(const __FlashStringHelper*const *prompts, unsigned char start, unsigned char count)
  {
    for (unsigned char i = start; i < count; i++) {
      // Add space for the selection dot
      display.print("   ");
      // Print menu item
      display.println((const __FlashStringHelper*const)pgm_read_word(&(prompts[i])));
    }
  }
};

template<class T>
void list_menu<T>::draw() {
  //clear the screen
  display.clear();
  display.set_color(draw_color::NORMAL);

  // print menu
  display.println(question);
  uint8_t current_page = choice / 7; // Max 7 options per page
  uint8_t prompts_on_page = min(7, num_prompts - current_page * 7);

  print_loop(prompts, current_page * 7, current_page * 7 + prompts_on_page);

  // draw selection box
  display.get_display().drawBox(1, 8 * (choice % 7) + 11, 3, 3);
  display.update();
}

template<class T>
void list_menu<T>::update(uint8_t new_choice) {
  if (new_choice == choice) {
    return;
  }
  if (new_choice > num_prompts) {
    new_choice = num_prompts;
  }

  uint8_t current_page = choice / 7; // Max 7 options per page
  uint8_t new_page = new_choice / 7;
  if (current_page != new_page) {
    // We need to redraw everything...
    choice = new_choice;
    draw();
  } else {
    // remove selection box
    display.set_color(draw_color::INVERTED);
    display.get_display().drawBox(1, 8 * (choice % 7) + 11, 3, 3);
    display.set_color(draw_color::NORMAL);
    
    choice = new_choice;

    // draw selection box
    display.get_display().drawBox(1, 8 * (choice % 7) + 11, 3, 3);
    display.update();
  }
}

template<class T>
class list_menu_controller {
public:
  list_menu_controller(list_menu<T>& model)
  :model(model), idle_time(millis())
  {}

  bool tick();

private:
  list_menu<T>& model;
  unsigned long idle_time;
};

typedef enum COLOR_T {
  blue_color,
  red_color,
  purple_color,
  green_color,
  turquoise_color,
  yellow_color,
  white_color,
  black_color,
} color_t;

// FIXME these should be somewhere else?
void rgbLed(byte Color);
uint8_t checkButton();

template<class T>
bool list_menu_controller<T>::tick() {
  // change the rgb led to the start menu color
  rgbLed(model.get_choice());

  byte currentColor = model.get_choice();

  /* Check Button/rotary encoder
  1 click/clockwise rotation
  2 doubleClick/counter clockwise rotation
  3 hold/press
  4 longHold */
  uint8_t b = checkButton();

  if (millis() - idle_time > 300000) {
    if ((millis() - idle_time) % 4000 == 0) {
      currentColor = (currentColor + 1) % 5;
      if (currentColor == red_color) {
        currentColor = 2;  // skip red as that signifies an error to the user
      }
      rgbLed(currentColor);
    }
  }

  if (!b)
    return false;

  idle_time = millis();

  byte choice = model.get_choice();
  // if button is pressed twice or rotary encoder turned left/counter clockwise
  if (b == 2) {
    if (choice == 0) {
      choice = model.max_choices() - 1;
    } else {
      choice--;
    }
    model.update(choice);
  }
  // go one down in the menu if the Cart Readers button is clicked shortly
  if (b == 1) {
    choice = (choice + 1) % model.max_choices();
    model.update(choice);
  }
  // if the Cart Readers button is hold continiously leave the menu
  // so the currently highlighted action can be executed

  if (b == 3 || b == 4) {
    rgbLed(black_color);
    return true;
  }

  rgbLed(model.get_choice());
  return false;
}

#endif//MENU_H_
