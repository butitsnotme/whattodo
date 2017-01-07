#include <cppcms/application.h>
#include <cppcms/service.h>
#include <cppcms/http_response.h>

namespace whattodo {

class whattodo: public cppcms::application {
public:
  whattodo(cppcms::service &srv);
  virtual void main(std::string url);
};

}

