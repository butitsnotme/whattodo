#include "whattodo/user.h"

#include <cstring>
#include <sodium.h>

cppdb::session whattodo::user::sql;

void whattodo::user::set_session(cppdb::session &session)
{
  user::sql = session;
}

whattodo::user::user(std::string username)
{
  this->username = username;
}

bool whattodo::user::create()
{
  user::sql
    << "INSERT OR IGNORE INTO users (username) VALUES (?)"
    << username
    << cppdb::exec;
  return true;
}

bool whattodo::user::set_password(std::string password)
{
  char hashed_password[crypto_pwhash_STRBYTES];
  if (0 != crypto_pwhash_str(
        hashed_password,
        password.c_str(),
        std::strlen(password.c_str()),
        crypto_pwhash_OPSLIMIT_INTERACTIVE,
        crypto_pwhash_MEMLIMIT_INTERACTIVE)) {
    return false;
  }
  user::sql
    << "UPDATE users SET password = ? WHERE username = ?"
    << hashed_password
    << username
    << cppdb::exec;
  return true;
}

bool whattodo::user::verify_password(std::string password)
{
  cppdb::result r = user::sql
    << "SELECT password FROM users WHERE username = ?"
    << username
    << cppdb::row;
  if (r.empty()) {
    return false;
  }

  std::string p = r.get<std::string>("password");
  char hashed_password[crypto_pwhash_STRBYTES];
  std::strcpy(hashed_password, p.c_str());
  if (0 != crypto_pwhash_str_verify(
        hashed_password,
        password.c_str(),
        std::strlen(password.c_str()))) {
    return false;
  } else {
    return true;
  }
}
