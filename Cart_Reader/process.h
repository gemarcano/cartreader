#ifndef PROCESS_H_
#define PROCESS_H_

#include "Cart_Reader.h"
#include "unique_ptr.h"
#include "menu.h"

#include <stdint.h>
#include <avr/io.h>
#include <U8g2lib.h>

class process
{
public:
  virtual ~process() = default;
  virtual bool tick() = 0;
  virtual void draw() = 0;
  virtual uint8_t get_choice() = 0;
  virtual unique_ptr<process> next_process(uint8_t /*choice*/)
  {
    return unique_ptr<process>();
  }
};

template<class T1, class T2>
class list_process : public process {
public:
  list_process(screen_display& display, screen_input& input, const T1* title, T2* prompts, uint8_t max_choices, uint8_t default_choice)
  :menu(title, prompts, max_choices, default_choice), view(display, menu), controller(menu, input, view)
  {
    view.draw();
  }

  virtual ~list_process() = default;

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


// FIXME where should this be?
unique_ptr<process> setup_menu(uint8_t);

template<class T2>
class main_process : public list_process<__FlashStringHelper, T2> {
public:
  main_process(screen_display& display, screen_input& input, const __FlashStringHelper* title, T2* prompts, uint8_t max_choices, uint8_t default_choice)
  :list_process<__FlashStringHelper, T2>(display, input, title, prompts, max_choices, default_choice)
  {}

  virtual ~main_process() = default;

  unique_ptr<process> next_process(uint8_t choice) {
    return setup_menu(choice);
  }
};

template<class Menu, class T1, class T2>
unique_ptr<process> build_process(screen_display& display, screen_input& input, const T1* title, T2* prompts, uint8_t max_choices, uint8_t default_choice = 0)
{
  return unique_ptr<process>(
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

#endif//PROCESS_H_
