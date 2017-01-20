{
  "whattodo" : {
    "connection": "sqlite3:db=${WHATTODO_DATA_DIRECTORY}/todo.db"
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
      "dir" : "/tmp/whattodo"
    },
    "gc" : 900
  },
  "logging" : {
    "level" : "warning",
    "syslog" : {
      "enable" : true
    }
  }
}

