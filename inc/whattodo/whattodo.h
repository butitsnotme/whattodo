#include <cppdb/frontend.h>
#include <cppcms/application.h>
#include <cppcms/service.h>
#include <cppcms/http_response.h>

#include <content/main.h>

namespace whattodo {

class whattodo: public cppcms::application {
public:
  whattodo(cppcms::service &srv);
  void index(std::string url);
  void login();
  void logout();
  void todo(std::string url);
  void new_todo();
  std::list<content::todo> get_list(int active = -1);
private:
  bool check_login(std::string username, std::string password);
  cppdb::session conn;
};

}

