#include "whattodo/todo.h"

#include <booster/log.h>

int whattodo::history_item::seq()
{
  return n_seq;
}

std::string whattodo::history_item::type()
{
  return s_type;
}

namespace whattodo {

template<typename T>
T history_item::value()
{
  return NULL;
}

template<>
std::string history_item::value()
{
  return (is_string ? s_val : std::to_string(n_val));
}

template<>
int history_item::value()
{
  return (is_string || is_todo ? 0 : n_val);
}

template<>
std::shared_ptr<todo> history_item::value()
{
  return t_val;
}

}

int whattodo::history_item::timestamp()
{
  return time;
}

cppdb::session whattodo::todo::sql;

void whattodo::todo::set_session(cppdb::session &session)
{
  todo::sql = session;
}

std::list<std::shared_ptr<whattodo::todo>> whattodo::todo::get_all(int userid)
{
  // Late ToDos
  cppdb::result r = todo::sql
    << "SELECT rowid FROM todos WHERE user = ? AND NOT due = 0 AND "
       "due < strftime('%s', 'now') AND NOT status = 'done' ORDER BY "
       "due ASC, rowid ASC"
    << userid;
  std::list<std::shared_ptr<todo>> l;
  while(r.next()) {
    l.push_back(std::make_shared<todo>(userid, r.get<int>("rowid")));
  }

  // ToDos with a due date, but no time estimate
  r = todo::sql
    << "SELECT rowid FROM todos WHERE user = ? AND NOT due = 0 AND "
       "estimate = 0 AND due > strftime('%s', 'now') AND "
       "status NOT IN('blocked', 'done') ORDER BY due DESC, rowid ASC"
    << userid;
  while(r.next()) {
    l.push_back(std::make_shared<todo>(userid, r.get<int>("rowid")));
  }

  // ToDos with both due date and time estimate
  r = todo::sql
    << "SELECT rowid FROM todos WHERE user = ? AND NOT due = 0 AND "
       "NOT estimate = 0 AND status NOT IN('blocked', 'done') AND "
       "due > strftime('%s', 'now') ORDER BY (due - strftime('%s', 'now')) / "
       "(estimate - worked) ASC, rowid ASC"
    << userid;
  while(r.next()) {
    l.push_back(std::make_shared<todo>(userid, r.get<int>("rowid")));
  }

  // ToDos with no due date ('someday')
  r = todo::sql
    << "SELECT rowid FROM todos WHERE user = ? AND due = 0 AND "
       "status NOT IN('blocked', 'done') ORDER BY rowid"
    << userid;
  while(r.next()) {
    l.push_back(std::make_shared<todo>(userid, r.get<int>("rowid")));
  }

  // ToDos which are blocked
  r = todo::sql
    << "SELECT rowid FROM todos WHERE user = ? AND status = 'blocked' AND "
       "(due = 0 OR due > strftime('%s', 'now')) ORDER BY rowid"
    << userid;
  while(r.next()) {
    l.push_back(std::make_shared<todo>(userid, r.get<int>("rowid")));
  }

  // ToDos which are done
  r = todo::sql
    << "SELECT rowid FROM todos WHERE user = ? AND status = 'done' "
       "ORDER BY rowid"
    << userid;
  while(r.next()) {
    l.push_back(std::make_shared<todo>(userid, r.get<int>("rowid")));
  }

  return l;
}

whattodo::todo::todo() :
  user(-1),
  id(-1)
{
}

whattodo::todo::todo(int user_id, int todo_id) :
  id(todo_id),
  user(user_id)
{
  cppdb::result r = todo::sql
    << "SELECT count(*) AS num FROM todos WHERE user = ? AND rowid = ?"
    << user
    << id
    << cppdb::row;
  if (1 == r.get<int>("num")) {
    valid = true;
  }
}

whattodo::todo::todo(int user_id, std::string title) :
  user(user_id)
{
  cppdb::result r = todo::sql
    << "SELECT count(*) AS num FROM users WHERE id = ?"
    << user
    << cppdb::row;
  if (1 == r.get<int>("num")) {
    valid = true;
  }

  todo::sql
    << "INSERT INTO todos (user, title) VALUES (?, ?)"
    << user
    << title
    << cppdb::exec;
  cppdb::result res = todo::sql
    << "SELECT last_insert_rowid() AS rowid"
    << cppdb::row;
  id = res.get<int>("rowid");
  add_history("created", title);
}

bool whattodo::todo::is_valid()
{
  return valid;
}

void whattodo::todo::set_title(std::string title)
{
  if (!valid || get_title() == title) {
    return;
  }

  todo::sql
    << "UPDATE todos SET title = ? WHERE user = ? AND rowid = ?"
    << title
    << user
    << id
    << cppdb::exec;
  add_history("title", title);
}

void whattodo::todo::set_description(std::string description)
{
  if (!valid || get_description() == description) {
    return;
  }

  todo::sql
    << "UPDATE todos SET description = ? WHERE user = ? AND rowid = ?"
    << description
    << user
    << id
    << cppdb::exec;
  add_history("description", description);
}

void whattodo::todo::set_due(int time)
{
  if (!valid || get_due() == time) {
    return;
  }

  todo::sql
    << "UPDATE todos SET due = ? WHERE user = ? AND rowid = ?"
    << time
    << user
    << id
    << cppdb::exec;
  add_history("due", time);
}

void whattodo::todo::set_parent(std::shared_ptr<whattodo::todo> parent)
{
  set_parent(parent->get_id());
}

void whattodo::todo::set_parent(int parent)
{
  if (!valid || get_parent()->get_id() == parent) {
    return;
  }

  todo t(user, parent);
  if (!t.is_valid()) {
    return;
  }

  todo::sql
    << "UPDATE todos SET parent = ? WHERE user = ? AND rowid = ?"
    << parent
    << user
    << id
    << cppdb::exec;
  add_history("parent", parent);
}

void whattodo::todo::set_estimated(int estimate)
{
  if (!valid || get_estimated() == estimate) {
    return;
  }

  todo::sql
    << "UPDATE todos SET estimate = ? WHERE user = ? AND rowid = ?"
    << estimate
    << user
    << id
    << cppdb::exec;
  add_history("estimate", estimate);
}

void whattodo::todo::set_percent(int percent)
{
  if (!valid || get_percent() == percent) {
    return;
  }

  todo::sql
    << "UPDATE todos SET percent = ? WHERE user = ? AND rowid = ?"
    << percent
    << user
    << id
    << cppdb::exec;
  add_history("percent", percent);
}

void whattodo::todo::set_status(std::string status)
{
  if (!valid || get_status() == status) {
    return;
  }

  todo::sql
    << "UPDATE todos SET status = ? WHERE user = ? AND rowid = ?"
    << status
    << user
    << id
    << cppdb::exec;
  add_history("status", status);
}

void whattodo::todo::set_priority(int priority)
{
  if (!valid || get_priority() == priority) {
    return;
  }

  todo::sql
    << "UPDATE todos SET priority = ? WHERE user = ? AND rowid = ?"
    << user
    << id
    << cppdb::exec;
  add_history("priority", priority);
}

void whattodo::todo::add_worked(int worked)
{
  if (!valid || get_worked() == worked) {
    return;
  }

  todo::sql
    << "UPDATE todos SET worked = worked + ? WHERE user = ? AND rowid = ?"
    << worked
    << user
    << id
    << cppdb::exec;
  add_history("worked", worked);
}

void whattodo::todo::add_worked(int hours, int minutes)
{
  add_worked(hours * 60 + minutes);
}

void whattodo::todo::add_comment(std::string comment)
{
  if (!valid) {
    return;
  }

  add_history("comment", comment);
}

void whattodo::todo::add_block(std::shared_ptr<whattodo::todo> t)
{
  add_block(t->get_id());
}

void whattodo::todo::add_block(int todo_id)
{
  if (!valid || has_block(todo_id)) {
    return;
  }

  todo t(user, todo_id);
  if (!t.is_valid()) {
    return;
  }

  todo::sql
    << "INSERT INTO blockedby (blocked, blocking) VALUES(?, ?)"
    << id
    << todo_id
    << cppdb::exec;
  add_history("block", todo_id);
  set_status("blocked");
}

void whattodo::todo::remove_block(std::shared_ptr<whattodo::todo> t)
{
  remove_block(t->get_id());
}

void whattodo::todo::remove_block(int todo_id)
{
  if (!valid || !has_block(todo_id)) {
    return;
  }

  todo t(user, todo_id);
  if (!t.is_valid()) {
    return;
  }

  todo::sql
    << "DELETE FROM blockedby WHERE bocked = ? AND blocking = ?"
    << id
    << todo_id
    << cppdb::exec;
  add_history("unblock", todo_id);

  if (has_block()) {
    return;
  }

  cppdb::result r = todo::sql
    << "SELECT content FROM history WHERE type = 'status' AND todo = ? "
       "AND NOT content = 'blocked' ORDER BY rowid DESC LIMIT 1"
    << id
    << cppdb::row;
  set_status(r.get<std::string>("content"));
}

std::string whattodo::todo::get_title()
{
  if (!valid) {
    return "";
  }

  cppdb::result res = todo::sql
    << "SELECT title FROM todos WHERE user = ? AND rowid = ?"
    << user
    << id
    << cppdb::row;
  std::string title;
  res >> title;
  return title;
}

std::string whattodo::todo::get_description()
{
  if (!valid) {
    return "";
  }

  cppdb::result res = todo::sql
    << "SELECT description FROM todos WHERE user = ? AND rowid = ?"
    << user
    << id
    << cppdb::row;
  std::string description;
  res >> description;
  return description;
}

std::string whattodo::todo::get_status()
{
  if (!valid) {
    return "";
  }

  cppdb::result res = todo::sql
    << "SELECT status FROM todos WHERE user = ? AND rowid = ?"
    << user
    << id
    << cppdb::row;
  std::string status;
  res >> status;
  return status;
}

int whattodo::todo::get_id()
{
  return id;
}

int whattodo::todo::get_due()
{
  if (!valid) {
    return 0;
  }

  cppdb::result res = todo::sql
    << "SELECT due FROM todos WHERE user = ? AND rowid = ?"
    << user
    << id
    << cppdb::row;
  int time = -1;
  res >> time;
  return time;
}

int whattodo::todo::get_worked()
{
  if (!valid) {
    return 0;
  }

  cppdb::result res = todo::sql
    << "SELECT worked FROM todos WHERE user = ? AND rowid = ?"
    << user
    << id
    << cppdb::row;
  int worked = 0;
  res >> worked;
  return worked;
}

int whattodo::todo::get_estimated()
{
  if (!valid) {
    return 0;
  }

  cppdb::result res = todo::sql
    << "SELECT estimated FROM todos WHERE user = ? AND rowid = ?"
    << user
    << id
    << cppdb::row;
  int estimate = 0;
  res >> estimate;
  return estimate;
}

int whattodo::todo::get_percent()
{
  if (!valid) {
    return 0;
  }

  cppdb::result res = todo::sql
    << "SELECT percent FROM todos WHERE user = ? AND rowid = ?"
    << user
    << id
    << cppdb::row;
  int percent = 0;
  res >> percent;
  return percent;
}

int whattodo::todo::get_priority()
{
  if (!valid) {
    return 0;
  }

  cppdb::result res = todo::sql
    << "SELECT priority FROM todos WHERE user = ? AND rowid = ?"
    << user
    << id
    << cppdb::row;
  int priority = 1;
  res >> priority;
  return priority;
}

std::shared_ptr<whattodo::todo> whattodo::todo::get_parent()
{
  if (!valid) {
    return std::make_shared<todo>();
  }

  cppdb::result res = todo::sql
    << "SELECT parent FROM todos WHERE user = ? AND rowid = ?"
    << user
    << id
    << cppdb::row;
  int parent = -1;
  res >> parent;
  if (-1 == parent) {
    return std::make_shared<todo>();
  }
  return std::make_shared<todo>(user, parent);
}

std::list<std::shared_ptr<whattodo::todo>> whattodo::todo::get_blocks()
{
  if (!valid) {
    std::list<std::shared_ptr<todo>> l;
    return l;
  }

  cppdb::result r = todo::sql
    << "SELECT blocking FROM blockedby WHERE blocked = ? ORDER BY blocking ASC"
    << id;
  std::list<std::shared_ptr<todo>> blocks;
  while(r.next()) {
    blocks.push_back(std::make_shared<todo>(user, r.get<int>("blocking")));
  }
  return blocks;
}

bool whattodo::todo::has_block(int todo_id)
{
  if (!valid) {
    return false;
  }

  cppdb::result res = todo::sql
    << "SELECT rowid FROM blockedby WHERE blocked = ? AND blocking = ?"
    << id
    << todo_id
    << cppdb::row;
  return !res.empty();
}

bool whattodo::todo::has_block()
{
  if (!valid) {
    return false;
  }

  cppdb::result res = todo::sql
    << "SELECT rowid FROM blockedby WHERE blocked = ?"
    << id;
  if (res.next()) {
    return true;
  }
  return false;
}

std::list<whattodo::history_item> whattodo::todo::get_history()
{
  if (!valid) {
    std::list<history_item> l;
    return l;
  }

  cppdb::result r = todo::sql
    << "SELECT type, content, timestamp FROM history WHERE todo = ? "
       "ORDER BY timestamp ASC"
    << id;
  std::list<history_item> history;
  int seq = 1;
  while(r.next()) {
    history_item h;

    h.n_seq = seq;
    h.s_type = r.get<std::string>("type");
    h.time = r.get<int>("timestamp");

    if ("created" == h.s_type ||
        "title" == h.s_type ||
        "description" == h.s_type ||
        "status" == h.s_type ||
        "comment" == h.s_type) {
        h.is_string = true;
    } else if (
        "parent" == h.s_type ||
        "block" == h.s_type ||
        "unblock" == h.s_type) {
        h.is_todo = true;
    }

    if (h.is_string) {
      h.s_val = r.get<std::string>("content");
    } else if (h.is_todo) {
      h.t_val = std::make_shared<todo>(user, r.get<int>("content"));
    } else {
      h.n_val = r.get<int>("content");
    }

    history.push_back(h);
    seq++;
  }
  return history;
}

std::list<std::shared_ptr<whattodo::todo>> whattodo::todo::get_children()
{
  std::list<std::shared_ptr<todo>> children;
  if (!valid) {
    return children;
  }

  cppdb::result r = todo::sql
    << "SELECT rowid FROM todos WHERE user = ? AND parent = ? ORDER BY rowid"
    << user
    << id;
  while(r.next()) {
    children.push_back(std::make_shared<todo>(user, r.get<int>("rowid")));
  }
  return children;
}

void whattodo::todo::add_history(std::string type, std::string value)
{
  if (!valid) {
    return;
  }

  todo::sql
    << "INSERT INTO history (todo, type, content, timestamp) "
       "VALUES (?, ?, ?, strftime('%s', 'now'))"
    << id
    << type
    << value
    << cppdb::exec;
}

void whattodo::todo::add_history(std::string type, int value)
{
  if (!valid) {
    return;
  }

  todo::sql
    << "INSERT INTO history (todo, type, content, timestamp) "
       "VALUES (?, ?, ?, strftime('%s', 'now'))"
    << id
    << type
    << value
    << cppdb::exec;
}

