#include "whattodo/whattodo.h"

#include <booster/log.h>
#include <cppcms/url_dispatcher.h>
#include <ctime>
#include <sstream>

#include "content/index.h"
#include "content/login.h"
#include "content/new.h"
#include "content/todo.h"
#include "whattodo/todo.h"

extern "C" {
#include "approxidate.h"
}

namespace whattodo {
std::string format_time(int time)
{
  std::time_t t_val = time;
  std::stringstream ss;
  ss
    << std::put_time(std::localtime(&t_val), "%c, %Z");
  return ss.str();
}
}

whattodo::whattodo::whattodo(cppcms::service &srv) :
    cppcms::application(srv),
    conn(settings().get<std::string>("whattodo.connection"))
{
  dispatcher().assign("/login", &whattodo::login, this);
  mapper().assign("login", "/login");

  dispatcher().assign("/logout", &whattodo::logout, this);
  mapper().assign("logout", "/logout");

  dispatcher().assign("/todo/(\\d+)", &whattodo::todo, this, 1);
  mapper().assign("todo", "/todo/{1}");

  dispatcher().assign("/new", &whattodo::new_todo, this);
  mapper().assign("new", "/new");

  dispatcher().assign("", &whattodo::index, this, 1);
  mapper().assign("");

  mapper().root("/whattodo");

  ::whattodo::todo::set_session(conn);
}

void whattodo::whattodo::index(std::string url)
{
  if (!session().is_set("id")) {
    response().set_redirect_header("/login");
    return;
  }

  content::index i;

  i.todos = get_list();

  i.page_title = "Home";
  render("index", i);
}

void whattodo::whattodo::login()
{
  content::login l;
  
  if ("POST" == request().request_method() && session().is_set("prelogin")) {
    l.form.load(context());
    if (l.form.validate() && 
        check_login(l.form.username.value(), l.form.password.value())) {
      session().reset_session();
      session().erase("prelogin");
      response().set_redirect_header(url("/"));
      return;
    } else {
      l.login_failed = true;
    }
  }

  session().set("prelogin", "");
  render("login", l);
}

void whattodo::whattodo::logout()
{
  session().erase("id");
  session().erase("user");
  session().reset_session();
  response().set_redirect_header(url("/"));
}

void whattodo::whattodo::todo(std::string url)
{
  if (!session().is_set("id")) {
    response().set_redirect_header("/login");
    return;
  }

  auto t = std::make_shared<::whattodo::todo>(
      std::stoi(session().get("id")),
      std::stoi(url));
  if (!t->is_valid()) {
    response().set_redirect_header("/");
    return;
  }

  content::todo_details td;
  td.id = t->get_id();
  td.todo_title = t->get_title();

  td.todos = get_list(td.id);

  auto blocks = t->get_blocks();
  for(auto &block : blocks) {
    content::display_item di;
    std::stringstream ss;
    ss << "Blocking: #" << block->get_id() << ": " << block->get_title();
    di.title = ss.str();
    di.type = "warning";
    di.text = block->get_description();
    td.items.push_back(di);
  }

  auto parent = t->get_parent();
  if (parent->is_valid()) {
    content::display_item di;
    std::stringstream ss;
    ss << "Parent: #" << parent->get_id() << ": " << parent->get_title();
    di.title = ss.str();
    di.type = "info";
    di.text = parent->get_description();
    td.items.push_back(di);
  }

  auto children = t->get_children();
  for(auto &child: children) {
    content::display_item di;
    std::stringstream ss;
    ss << "Child: #" << child->get_id() << ": " << child->get_title();
    di.title = ss.str();
    di.type = "info";
    di.text = child->get_description();
    td.items.push_back(di);
  }

  auto history = t->get_history();
  for(auto &h : history) {
    std::stringstream ss;
    content::display_item di;
    ss << h.seq() << " - ";
    if ("created" == h.type()) {
      ss << "Created";
      di.type = "success";
      di.text = h.value<std::string>();
    } else if ("title" == h.type()) {
      ss << "Changed title";
      di.type = "warning";
      di.text = h.value<std::string>();
    } else if ("description" == h.type()) {
      ss << "Changed description";
      di.type = "default";
      di.text = h.value<std::string>();
    } else if ("due" == h.type()) {
      ss << "Changed due date";
      di.type = "default";
      std::stringstream ss2;
      ss2 << "Due date changed to: " << format_time(h.value<int>()) << ".";
      di.text = ss2.str();
    } else if ("parent" == h.type()) {
      auto p = h.value<std::shared_ptr<::whattodo::todo>>();
      ss << "Changed parent: #" << p->get_id() << ": " << p->get_title();
      di.type = "default";
      di.text = p->get_description();
    } else if ("estimate" == h.type()) {
      ss << "Changed estimated completion time";
      di.type = "default";
      int hours = h.value<int>() / 60;
      int mins = h.value<int>() % 60;
      std::stringstream ss2;
      ss2 << "Estimated total time to complete the task: " << hours
          << " hours and " << mins << " minutes.";
      di.text = ss2.str();
    } else if ("percent" == h.type()) {
      ss << "Changed the percentage complete";
      di.type = "default";
      std::stringstream ss2;
      ss2 << "Estimated " << h.value<int>() << "% complete.";
      di.text = ss2.str();
    } else if ("status" == h.type()) {
      ss << "Changed the status";
      di.type = "default";
      di.text = "Status changed to " + h.value<std::string>() + ".";
    } else if ("priority" == h.type()) {
      ss << "Changed the priority";
      di.type = "default";
      std::stringstream ss2;
      ss2 << "Priority changed to " << h.value<int>() << ".";
      di.text = ss2.str();
    } else if ("worked" == h.type()) {
      ss << "Added time worked";
      di.type = "success";
      int hours = h.value<int>() / 60;
      int mins = h.value<int>() % 60;
      std::stringstream ss2;
      ss2 << "Worked on the task for " << hours
          << " hours and " << mins << " minutes.";
      di.text = ss2.str();
    } else if ("comment" == h.type()) {
      ss << "Added a comment";
      di.type = "info";
      di.text = h.value<std::string>();
    } else if ("block" == h.type()) {
      auto p = h.value<std::shared_ptr<::whattodo::todo>>();
      ss << "Added a blocking todo: #" << p->get_id() << ": " << p->get_title();
      di.type = "warning";
      di.text = p->get_description();
    } else if ("unblock" == h.type()) {
      auto p = h.value<std::shared_ptr<::whattodo::todo>>();
      ss << "Removed a blocking todo: #" << p->get_id() << ": " << p->get_title();
      ss << "";
      di.type = "success";
      di.text = p->get_description();
    }
    di.title = ss.str();
    di.formatted_time = format_time(h.timestamp());
    td.items.push_back(di);
  }

  td.page_title = "Details";
  render("todo_details", td);
}

void whattodo::whattodo::new_todo()
{
  if (!session().is_set("id")) {
    response().set_redirect_header("/login");
    return;
  }

  content::new_todo t;
  t.page_title = "New ToDo";
  t.error = false;
  if ("POST" == request().request_method()) {
    t.form.load(context());
    if (t.form.validate()) {
      auto td = std::make_shared<::whattodo::todo>(
          std::stoi(session().get("id")),
          t.form.todo.value());
      if (!td->is_valid()) {
        BOOSTER_DEBUG("whattodo") << "ToDo object not valid";
        t.error = true;
        render("new_todo", t);
        return;
      }
      BOOSTER_DEBUG("whattodo") << "ToDo object is valid";
      if (t.form.description.set()) {
        td->set_description(t.form.description.value());
      }
      if (t.form.due.set()) {
        td->set_due(t.form.due_time);
      }
      if (t.form.parent.set()) {
        td->set_parent(t.form.parent.value());
      }
      if (t.form.hours.set()) {
        td->set_estimated(t.form.hours.value());
      }
      if (t.form.priority.set() && -1 != t.form.priority.selected()) {
        td->set_priority(t.form.priority.selected());
      }
      td->set_status("open");
      response().set_redirect_header(url("/todo", td->get_id()));
    } else {
      BOOSTER_DEBUG("whattodo") << "Form parameters not valid";
    }
  }

  t.todos = get_list();

  render("new_todo", t);
}

bool whattodo::whattodo::check_login(std::string username, std::string password)
{
  cppdb::result res = conn << "SELECT id, salt, password FROM users WHERE "
   "username = ?" << username << cppdb::row;

  if(!res.empty()) {
    int id = res.get<int>("id");
    std::string salt = res.get<std::string>("salt");
    std::string hash = res.get<std::string>("password");

    if (hash == password) {
      session().set("id", id);
      session().set("user", username);
      return true;
    }
  }
  return false;
}

std::list<content::todo> whattodo::whattodo::get_list(int active)
{
  std::list<content::todo> l;

  struct timeval tv;
  approxidate("now", &tv);
  int now = tv.tv_sec;

  auto todos = todo::get_all(std::stoi(session().get("id")));
  for(auto &t: todos) {
    content::todo ct;
    ct.id = t->get_id();
    ct.title = t->get_title();
    ct.description = t->get_description();
    std::string status = t->get_status();
    if("blocked" == status) {
      ct.type = "warning";
    } else {
      ct.type = "info";
    }

    int due = t->get_due();
    if (now > due && 0 != due) {
      ct.type = "danger";
    }

    if ("done" == status) {
      ct.type = "success";
    }
    if (ct.id == active) {
      ct.type += " active";
    }
    l.push_back(ct);
  }

  return l;
}

