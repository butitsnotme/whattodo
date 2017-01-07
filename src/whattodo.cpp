#include "whattodo/whattodo.h"

whattodo::whattodo::whattodo(cppcms::service &srv) :
    cppcms::application(srv)
{
}

void whattodo::whattodo::main(std::string url)
{
  response().out() << "<html><head><title>whatTODO</title></head><body>"
    "<h1>whatTODO</h1></body></html>";
}

