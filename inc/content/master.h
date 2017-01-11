#pragma once

#include <cppcms/view.h>
#include <string>

namespace content {
  
struct master : public cppcms::base_content {
  std::string title;
  master()
  {
    title = "whatTODO";
  }
};

}
