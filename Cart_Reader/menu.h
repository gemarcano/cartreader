#ifndef MENU_H_
#define MENU_H_

#include "Cart_Reader.h"
#include "unique_ptr.h"

#include <stdint.h>
#include <avr/io.h>
#include <U8g2lib.h>

/** Represents the contents and state of menus.
 *
 * @tparam T1 The type of the title (typically __FlashStringHelper or
 *  const char *)
 * @tparam T2 The type of the list of prompts (typically
 *  __FlashStringHelper*const, although there's a special case for
 *  char for strings that are in one array, '\0' separated)
 *
 */
template<class T1, class T2>
class menu_model {
public:
  /** Constructor.
   * 
   * @param[in] title Pointer to the menu title string.
   * @param[in] prompts Pointer to a list of prompt strings (or a special const
   *  char array that contains the strings consecutively, null separated),
   * @param[in] max_choices The number of prompts.
   * @param[in] default_choice The choice to use as a default.
   */
  menu_model(const T1* title, const T2* prompts, uint8_t max_choices, uint8_t default_choice)
  :title(title), prompts(prompts), max_choices_(max_choices), choice(default_choice)
  {} 

  /** Updates the menu option selected.
   *
   * If the provided choice is greater than the maximum number of choices
   * available, the last menu choice is used.
   *
   * @param[in] choice_ Position of the new choice to select.
   */
  void update(uint8_t choice_) {
    choice = min(choice_, max_choices_ - 1);
  }

  /** Returns the current choice.
   *
   * @returns The current choice.
   */
  uint8_t get_choice() const
  {
    return choice;
  }

  /** Returns the maximum number of menu choices available.
   *
   * @returns The maximum number of menu choices available.
   */
  uint8_t max_choices() const
  {
    return max_choices_;
  }

  /** Returns a pointer to the current title.
   *
   * @returns A pointer to the current title.
   */
  const T1* get_title() const
  {
    return title;
  }

  /** Returns a pointer to the prompts.
   *
   * @returns A pointer to the prompts.
   */ 
  const T2* get_prompts() const
  {
    return prompts;
  }

private:
  const T1*const title;
  const T2*const prompts;
  const uint8_t max_choices_;
  uint8_t choice;
};

/** Represents the view of a list menu.
 */
template<class T>
class list_menu_view {
public:
  list_menu_view(screen_display& display, const menu_model<__FlashStringHelper, T>& menu_)
  :menu_(menu_), display(display), old_choice(menu_.get_choice())
  {}

  virtual ~list_menu_view()
  {}

  void draw();
  void update(uint8_t choice);

private:
  const menu_model<__FlashStringHelper, T>& menu_;
  screen_display& display;

  uint8_t old_choice;

  template<class T2>
  void print_loop(const T2*const prompts, unsigned char start, unsigned char count)
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

  void print_loop(const char*const prompts, unsigned char start, unsigned char count)
  {
    const char* current = prompts;
    for (unsigned char i = 0; i < start; ++i) {
      current += strlen(current) + 1;
    }
    for (unsigned char i = start; i < count; i++) {
      // Add space for the selection dot
      display.print("   ");
      // Print menu item
      display.println(current);
      current += strlen(current) + 1;
    }
  }
};

template<class T>
void list_menu_view<T>::draw() {
  //clear the screen
  display.clear();
  display.set_color(draw_color::NORMAL);

  // print menu
  display.println(menu_.get_title());
  uint8_t current_page = menu_.get_choice() / 7; // Max 7 options per page
  uint8_t prompts_on_page = min(7, menu_.max_choices() - current_page * 7);

  print_loop(menu_.get_prompts(), current_page * 7, current_page * 7 + prompts_on_page);

  // draw selection box
  display.get_display().drawBox(1, 8 * (menu_.get_choice() % 7) + 11, 3, 3);
  display.update();
}

template<class T>
void list_menu_view<T>::update(uint8_t new_choice) {
  if (new_choice > menu_.max_choices()) {
    new_choice = menu_.max_choices();
  }

  if (new_choice == old_choice) {
    return;
  }

  uint8_t current_page = old_choice / 7; // Max 7 options per page
  uint8_t new_page = new_choice / 7;
  
  if (current_page != new_page) {
    // We need to redraw everything...
    draw();
  } else {
    // remove selection box
    display.set_color(draw_color::INVERTED);
    display.get_display().drawBox(1, 8 * (old_choice % 7) + 11, 3, 3);
    display.set_color(draw_color::NORMAL);

    // draw selection box
    display.get_display().drawBox(1, 8 * (new_choice % 7) + 11, 3, 3);
    display.update();
  }
  old_choice = new_choice;
}

// FIXME these should be somewhere else?
void rgbLed(byte Color);

template<class T>
class list_menu_controller {
public:
  list_menu_controller(menu_model<__FlashStringHelper, T>& menu_, screen_input& input, list_menu_view<T>& view)
  :model(menu_), input(input), view(view), idle_time(millis()), idle_choice(0)
  {
    rgbLed(menu_.get_choice());
  }

  bool tick();

private:
  menu_model<__FlashStringHelper, T>& model;
  screen_input input;
  list_menu_view<T>& view;
  unsigned long idle_time;
  uint8_t idle_choice;
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

template<class T>
bool list_menu_controller<T>::tick() {
  input.tick();
  // change the rgb led to the start menu color
  //rgbLed(model.get_choice());

  uint8_t event_flags = input.current_input_event();
  
  if (millis() - idle_time > 300000ul) {
    if ((millis() - idle_time) > (300000ul + 4000ul)) {
      idle_choice = (idle_choice + 1) % 5;
      if (idle_choice == red_color) {
        idle_choice = 2;  // skip red as that signifies an error to the user
      }
      rgbLed(idle_choice);
      idle_time += 4000;
    }
  }

  if (event_flags == 0)
    return false;

  idle_time = millis();
  
  byte choice = model.get_choice();
  if (event_flags & INPUT_BUTTON_PRESS) {
    // Update color for long press (FIXME what's the chance of millis wrapping?)
    if ((millis() - input.get_button_press_time()) > 2000) {
      rgbLed(green_color);
    } else {
      rgbLed(black_color);
    }
  } else if (event_flags & (INPUT_BUTTON_SHORT_RELEASE | INPUT_BUTTON_LONG_RELEASE)) {
    return true;
  }

  if (event_flags & INPUT_ROTARY_POSITIVE) {
    choice = (choice + 1) % model.max_choices();
    model.update(choice);
    view.update(choice);
    rgbLed(choice);
  } else if (event_flags & INPUT_ROTARY_NEGATIVE) {
    if (choice == 0) {
      choice = model.max_choices() - 1;
    } else {
      choice--;
    }
    model.update(choice);
    view.update(choice);
    rgbLed(choice);
  }

  return false;
}


class mvc_menu
{
public:
  virtual ~mvc_menu() = default;
  virtual bool tick() = 0;
  virtual void draw() = 0;
  virtual uint8_t get_choice() = 0;
  virtual unique_ptr<mvc_menu> process_choice(uint8_t /*choice*/)
  {
    return unique_ptr<mvc_menu>();
  }
};

template<class T1, class T2>
class mvc_list_menu : public mvc_menu {
public:
  mvc_list_menu(screen_display& display, screen_input& input, const T1* title, T2* prompts, uint8_t max_choices, uint8_t default_choice)
  :menu(title, prompts, max_choices, default_choice), view(display, menu), controller(menu, input, view)
  {
    view.draw();
  }

  virtual ~mvc_list_menu() = default;

  void draw() override {
    view.draw();
  }

  bool tick() override {
    return controller.tick();
  }

  uint8_t get_choice() override {
    return menu.get_choice();
  }

private:
  menu_model<T1, T2> menu;
  list_menu_view<T2> view;
  list_menu_controller<T2> controller;
};

template<class T2>
class mvc_main_menu : public mvc_list_menu<__FlashStringHelper, T2> {
public:
  mvc_main_menu(screen_display& display, screen_input& input, const __FlashStringHelper* title, T2* prompts, uint8_t max_choices, uint8_t default_choice)
  :mvc_list_menu<__FlashStringHelper, T2>(display, input, title, prompts, max_choices, default_choice)
  {}

  virtual ~mvc_main_menu() = default;

  unique_ptr<mvc_menu> process_choice(uint8_t choice) {
    return setup_menu(choice);
  }
};

template<class Menu, class T1, class T2>
unique_ptr<mvc_menu> build_menu(screen_display& display, screen_input& input, const T1* title, T2* prompts, uint8_t max_choices, uint8_t default_choice = 0)
{
  return unique_ptr<mvc_menu>(
    new Menu(
      display,
      input,
      title,
      prompts,
      max_choices,
      default_choice
    )
  );
}

#endif//MENU_H_
