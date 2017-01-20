# whatTODO
whatTODO is a web based To Do list. It was written for the purpose of being
simple and fast, with a focus on useability.

# Requirements
whatTODO runs on a Linux server. It requires that cppdb and cppcms shared
libraries are available. It also requires the sqlite3 binary and systemd (for
autostart).

# Installation
You can find packaged versions of cppdb and cppcms on
[github](https://github.com/whattodo/releases) in this project's releases
section. These (or equivalent) must be installed. Next install the debian
package containing whatTODO (also found on this project's releases page).
Finally, set up the database and start the software.

```bash
sudo apt-get install -y sqlite3
sudo ./cppdb-linux.sh --prefix=/usr --exclude-subdir
sudo ./cppcms-linux.sh --prefix=/usr --exclude-subdir
sudo useradd -rmU -d /var/lib/whattodo
sudo dpkg -i whattodo-*.deb
sudo sqlite3 /var/lib/whattodo/todo.db ".read /usr/share/whattodo/sqlite3.sql"
sudo chown whattodo:root /var/lib/whattodo/todo.db
sudo chmod 660 /var/lib/whattodo/todo.db
sudo whattodoctl adduser your-name-here
sudo systemctl enable whattodo
sudo systemctl start whattodo
```

Now navigate to `http://<installed-host>:8080/whattodo` to view test it out

