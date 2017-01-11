#pragma once

#include <cppcms/view.h>
#include <cppcms/form.h>
#include <iostream>

#include "content/master.h"

namespace content {

struct login_form : public cppcms::form {
  cppcms::widgets::text username;
  cppcms::widgets::password password;
  cppcms::widgets::submit submit;

  login_form()
  {
    username.message("Username");
    password.message("Password");
    submit.value("Login");
    form::add(username);
    form::add(password);
    form::add(submit);
  }
};

struct login : public master {
  std::string user, pass, page_title;
  bool login_failed;
  login_form form;
  login()
  {
    page_title = "Login";
    login_failed = false;
  }
};

}
