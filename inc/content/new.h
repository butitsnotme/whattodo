#pragma once

#include <cppcms/form.h>
#include <string>
#include <sstream>
#include <locale>
#include <iomanip>
#include <ctime>

extern "C" {
#include "approxidate.h"
}
#include "content/main.h"

namespace content {

struct todo_create_form : public cppcms::form {
  cppcms::widgets::text todo;
  cppcms::widgets::textarea description;
  cppcms::widgets::text due;
  cppcms::widgets::numeric<int> parent;
  cppcms::widgets::numeric<int> hours;
  cppcms::widgets::select priority;
  cppcms::widgets::submit create;
  int due_time;

  todo_create_form()
  {
    due_time = 0;
    todo.message("ToDo");
    description.message("Details");
    due.message("Due date");
    parent.message("Parent ToDo");
    hours.message("Time estimate (minutes)");
    create.value("Create");

    priority.message("Priority");
    for(int i = 1; i <= 5; ++i) {
      priority.add(std::to_string(i), std::to_string(i));
    }

    add(todo);
    add(description);
    add(due);
    add(parent);
    add(hours);
    add(priority);
    add(create);
  }

  virtual bool validate()
  {
    if (!form::validate()) {
      return false;
    }

    if (!todo.set()) {
      todo.valid(false);
      return false;
    }

    if (due.set() && "" != due.value()) {
      struct timeval tv;
      const char *dv = due.value().c_str();
      if (0 != approxidate(dv, &tv)) {
        due.valid(false);
        return false;
      }
      due_time = tv.tv_sec;
    }
    return true;
  }
};

struct new_todo : public main {
  std::string todo;
  std::string description;
  std::string due;
  double hours;
  int parent;
  bool error;

  todo_create_form form;
};

}
