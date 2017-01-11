#pragma once

#include "content/master.h"

namespace content {

struct todo : public cppcms::base_content {
  int id;
  std::string title;
  std::string description;
  std::string type;
};

struct main : public master {
  std::list<todo> todos;
  std::string page_title;
  std::string username;
};

}
