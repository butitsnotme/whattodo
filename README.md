# whatTODO
whatTODO is a web based To Do list. It was written for the purpose of being
simple and fast, with a focus on useability.

# Requirements
whatTODO runs on a Linux server. It requires that cppdb and cppcms shared
libraries are available. It also requires the sqlite3 binary and systemd (for
autostart).

# Installation
You can find packaged versions of cppdb and cppcms on [githubin this project's
releases section.](https://github.com/whattodo/releases)  These (or equivalent)
must be installed. Next install the debian package containing whatTODO (also
found on this project's releases page). Finally, add a user to the database

```bash
sudo ./cppdb-linux.sh --prefix=/usr --exclude-subdir
sudo ./cppcms-linux.sh --prefix=/usr --exclude-subdir
sudo dpkg -i whattodo-*.deb # At the very least, queue the package for
                            #   installation
sudo apt-get install -fy # Make sure that dependencies are installed
sudo whattodoctl adduser <your-name-here> # Add a user
```

> Note that you should replace `<your-name-here>` with a desired username

Now navigate to `http://<installed-host>:8080/whattodo` to view test it out

# Libraries used in this project

 * [args](https://github.com/Taywee/args) - Argument parsing in whattodoctl
 * [cppcms](http://cppcms.com/wikipp/en/page/main) - Web framework
 * [cppdb](http://cppcms.com/sql/cppdb/) - Database access
 * [libsodium](https://github.com/jedisct1/libsodium) - Password Hashing
