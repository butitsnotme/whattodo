#pragma once

#include <cppdb/frontend.h>
#include <string>
#include <list>
#include <memory>

namespace whattodo {

class todo;

class history_item {
  friend todo;
public:
  int seq();
  std::string type();
  template<typename T>
    T value();
  int timestamp();
private:
  bool is_string = false;
  bool is_todo = false;
  int n_seq;
  std::string s_type;
  std::string s_val;
  int n_val;
  std::shared_ptr<todo> t_val;
  int time;
};

class todo {
public:
  static void set_session(cppdb::session &session);
  static std::list<std::shared_ptr<todo>> get_all(int userid);

  todo();
  todo(int user_id, std::string title);
  todo(int user_id, int todo_id);

  bool is_valid();
  
  void set_title(std::string title);
  void set_description(std::string description);
  void set_due(int time);
  void set_parent(std::shared_ptr<todo> parent);
  void set_parent(int parent);
  void set_estimated(int estimate);
  void set_percent(int percent);
  void set_status(std::string status);
  void set_priority(int priority);

  void add_worked(int worked);
  void add_worked(int hours, int minutes);
  void add_comment(std::string comment);

  void add_block(std::shared_ptr<todo> t);
  void add_block(int todo_id);
  void remove_block(std::shared_ptr<todo> t);
  void remove_block(int todo_id);

  std::string get_title();
  std::string get_description();
  std::string get_status();
  int get_id();
  int get_due();
  int get_worked();
  int get_estimated();
  int get_percent();
  int get_priority();
  std::shared_ptr<todo> get_parent();

  std::list<std::shared_ptr<todo>> get_blocks();
  bool has_block(int todo_id);
  bool has_block();

  std::list<history_item> get_history();
  
  std::list<std::shared_ptr<todo>> get_children();
private:
  static cppdb::session sql;
  bool valid = false;
  int id = 0;
  int user = 0;

  void add_history(std::string type, std::string value);
  void add_history(std::string type, int value);
};

}
