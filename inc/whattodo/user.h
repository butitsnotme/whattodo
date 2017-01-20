#pragma once

#include <cppdb/frontend.h>
#include <string>

namespace whattodo {

class user {
public:
  static void set_session(cppdb::session &session);
  user(std::string);

  bool create();
  bool set_password(std::string password);
  bool verify_password(std::string password);
private:
  static cppdb::session sql;
  std::string username;
};

}
