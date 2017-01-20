#pragma once

#include <booster/log.h>
#include <cppcms/form.h>
#include <memory>

#include "content/main.h"
#include "whattodo.h"
#include "whattodo/todo.h"

namespace content {

struct todo_form : public cppcms::form {
  cppcms::widgets::text status;
  cppcms::widgets::text title;
  cppcms::widgets::text description;
  cppcms::widgets::text due;
  cppcms::widgets::numeric<int> parent;
  cppcms::widgets::numeric<int> estimated;
  cppcms::widgets::numeric<int> priority;
  cppcms::widgets::text blocking;
  cppcms::widgets::numeric<int> worked;
  cppcms::widgets::text comment;
  int user;
  int due_time;
  std::list<std::shared_ptr<whattodo::todo>> blocks;
  std::shared_ptr<whattodo::todo> t_parent;

  todo_form()
  {
    status.name("status");
    title.name("title");
    description.name("description");
    due.name("due");
    parent.name("parent");
    estimated.name("estimated");
    priority.name("priority");
    blocking.name("blocking");
    worked.name("worked");
    comment.name("comment");

    parent.low(0);
    estimated.low(0);
    priority.range(1, 5);
    worked.low(0);

    form::add(status);
    form::add(title);
    form::add(description);
    form::add(due);
    form::add(parent);
    form::add(estimated);
    form::add(priority);
    form::add(blocking);
    form::add(worked);
    form::add(comment);
  }

  virtual bool validate()
  {
    if (!form::validate()) {
      return false;
    }

    if (due.set() && "" != due.value()) {
      int time = whattodo::get_time(due.value());
      if (-1 == time) {
        due.valid(false);
        return false;
      }
      due_time = time;
    } else {
      due_time = 0;
    }

    if (parent.set()) {
      t_parent = std::make_shared<whattodo::todo>(user, parent.value());
      if (!t_parent->is_valid() && 0 != parent.value()) {
        return false;
      }
    }

    if (blocking.set() && "" != blocking.value()) {
      std::stringstream ss(blocking.value());
      std::string block;
      while(std::getline(ss, block, ' ')) {
        if ("" == block) {
          continue;
        }
        std::shared_ptr<whattodo::todo> t =
          std::make_shared<whattodo::todo>(user, std::stoi(block));
        if (!t->is_valid()) {
          return false;
        }
        blocks.push_back(t);
      }
    }

    if (status.set() && "" != status.value()) {
      if ("done" != status.value() &&
          "blocked" != status.value() &&
          "open" != status.value()) {
        return false;
      }
    } else {
      status.set("open");
    }

    return true;
  }
};

struct display_item : public cppcms::base_content {
  std::string formatted_time;
  std::string type;
  std::string title;
  std::string text;
};

struct todo_details : public main {
  int id;
  std::string todo_title;
  std::string description;
  std::string status;
  std::string due;
  int parent;
  int estimated;
  int priority;
  std::string worked;
  std::string blocking;
  std::list<display_item> items;
  todo_form form;
  bool error;

  todo_details()
  {
    error = false;
  }
};

}
