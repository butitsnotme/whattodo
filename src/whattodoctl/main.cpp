#include <termios.h>
#include <unistd.h>
#include <iostream>
#include <regex>
#include <sodium.h>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include "args/args.hxx"
#include "data.h"
#include "whattodo/user.h"

DATA_FILE(license)
DATA_FILE(version)
DATA_FILE(version_tmpl)

enum class subcommands {
  none,
  add_user
};

void set_echo(bool enable)
{
  struct termios tty;
  tcgetattr(STDIN_FILENO, &tty);
  if( !enable ) {
    tty.c_lflag &= ~ECHO;
  } else {
    tty.c_lflag |= ECHO;
  }

  tcsetattr(STDIN_FILENO, TCSANOW, &tty);
}

int main(int argc, char** argv)
{
  std::string prog_name = "whattodoctl";
  std::string description = "Manages the whattodo server database";

  // Convert the arguments to a vector
  std::vector<std::string> args(argv + 1, argv + argc);

  // Subcommand mapping
  std::unordered_map<std::string, subcommands> subs{
    {"adduser", subcommands::add_user}
  };

  ::args::ArgumentParser main(description);
  main.Prog(argv[0]);
  ::args::Flag license(main, "license", "Show the license", {'l', "license"});
  ::args::Flag help(main, "help", "Display this help text", {'h', "help"});
  ::args::Flag version(main, "version", "Show the program version", {'v', "version"});
  ::args::Flag warranty(main, "warranty", "Show the program warranty information",
      {'w', "warranty"});
  ::args::MapPositional<std::string, subcommands> subcommand(main, "SUBCOMMAND",
      "Subcommand to run", subs);
  subcommand.KickOut(true);

  std::vector<std::string>::const_iterator next = std::end(args);
  try {
    next = main.ParseArgs(args.begin(), args.end());
  } catch(::args::ParseError e) {
    std::cerr << main;
    return 1;
  }

  if (help) {
    std::cout << main;
  }

  if (version) {
    std::stringstream ssVersion(&version_begin);
    std::vector<std::string> vVersion;
    std::string line;
    while(getline(ssVersion, line)) {
      vVersion.push_back(line);
    }

    // Select the template
    std::string v(&version_tmpl_begin);

    // Fill in the template
    v = regex_replace(v, std::regex("<PROG_NAME>"), prog_name);
    v = regex_replace(v, std::regex("<VERSION>"), vVersion.at(0));
#ifdef DEVELOPMENT_BUILD
    v = regex_replace(v, std::regex("<GIT_HASH>"), vVersion.at(1));
    v = regex_replace(v, std::regex("<GIT_BRANCH>"), vVersion.at(2));
    v = regex_replace(v, std::regex("<BUILDER>"), vVersion.at(3));
    v = regex_replace(v, std::regex("<DATE>"), vVersion.at(4));
#endif

    std::cout << v;
  }

  if (license) {
    std::cout << &license_begin << std::endl;
  }

  if (warranty && !license) {
    std::string l(&license_begin);
    size_t begin = l.find("  15.");
    size_t end = l.find("END OF TERMS AND CONDITIONS");
    size_t length = end - begin;
    std::string w = l.substr(begin, length);
    w.erase(w.find_last_not_of(" \n\r\t")+1); 
    std::cout << w << std::endl;
  }

  if (help || version || license || warranty) {
    return 0;
  }

  if (!subcommand) {
    return 1;
  }
  
  if (subcommands::none == ::args::get(subcommand)) {
    return 1;
  }

  if (subcommands::add_user == ::args::get(subcommand)) {
    ::args::ArgumentParser add_user(static_cast<std::string>(argv[0]) +
        static_cast<std::string>(" adduser"));
    ::args::Flag auHelp(add_user, "help", "Help for adduser subcommand",
        {'h', "help"});
    ::args::Positional<std::string> user(add_user, "user name",
        "The name of the user to add");

    std::vector<std::string>::const_iterator arg_end = std::end(args);
    try {
      add_user.ParseArgs(next, arg_end);
    } catch (::args::ParseError e) {
      std::cerr << add_user << std::endl;
      return 1;
    }

    if (auHelp) {
      std::cout << add_user;
      return 0;
    }

    std::string passwd;
    std::string confirm;
    std::cout << "Enter password: ";
    set_echo(false);
    std::getline(std::cin, passwd);
    set_echo(true);
    std::cout << std::endl << "Retype password: ";
    set_echo(false);
    std::getline(std::cin, confirm);
    set_echo(true);
    std::cout << std::endl;

    if (passwd != confirm) {
      std::cerr << "Passwords don't match, no password set";
      return 2;
    }

    if (-1 == sodium_init()) {
      return 3;
    }

    cppdb::session sql("sqlite3:db=${WHATTODO_DATA_DIRECTORY}/todo.db");
    whattodo::user::set_session(sql);

    whattodo::user u(::args::get(user));
    u.create();
    u.set_password(passwd);
    return 0;
  }
};
