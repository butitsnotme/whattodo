#pragma once

#include "content/main.h"

namespace content {

struct display_item : public cppcms::base_content {
  std::string formatted_time;
  std::string type;
  std::string title;
  std::string text;
};

struct todo_details : public main {
  int id;
  std::string todo_title;
  std::list<display_item> items;
};

}
