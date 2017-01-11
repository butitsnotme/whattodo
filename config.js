{
  "whattodo" : {
    "connection": "sqlite3:db=todo.db"
  },
  "service" : {
    "api" : "http",
    "ip" : "0.0.0.0",
    "port" : 8080
  },
  "http" : {
    "script" : "/whattodo"
  },
  "security" : {
    "csrf" : {
      "enable" : true
    }
  },
  "session" : {
    "location" : "server",
    "timeout" : 86400,
    "expire" : "fixed",
    "cookies" : {
      "prefix" : "whattodo",
    },
    "server" : {
      "storage" : "files",
      "dir" : "/tmp/whattodo/sessions"
    },
    "gc" : 900
  }
}

