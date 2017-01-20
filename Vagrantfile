# -*- mode: ruby -*-
# vi: set ft=ruby :

# All Vagrant configuration is done below. The "2" in Vagrant.configure
# configures the configuration version (we support older styles for
# backwards compatibility). Please don't change it unless you know what
# you're doing.
Vagrant.configure(2) do |config|
  # The most common configuration options are documented and commented below.
  # For a complete reference, please see the online documentation at
  # https://docs.vagrantup.com.

  # Every Vagrant development environment requires a box. You can search for
  # boxes at https://atlas.hashicorp.com/search.
  config.vm.box = "ubuntu/xenial64"

  config.vm.define "test", autostart: false do |test|
    test.vm.network "forwarded_port", guest: 8080, host: 8081

    test.vm.provision "shell", inline: <<-SHELL
      apt-get update
      apt-get install -y sqlite3
      useradd -rmU -d /var/lib/whattodo whattodo
      chown :root /var/lib/whattodo
      chmod 770 /var/lib/whattodo
    SHELL
  end

  config.vm.define "dev", primary: true do |dev|
    # Create a forwarded port mapping which allows access to a specific port
    # within the machine from a port on the host machine. In the example below,
    # accessing "localhost:8080" will access port 80 on the guest machine.
    dev.vm.network "forwarded_port", guest: 8080, host: 12345
  
    # Enable provisioning with a shell script. Additional provisioners such as
    # Puppet, Chef, Ansible, Salt, and Docker are also available. Please see the
    # documentation for more information about their specific syntax and use.
    dev.vm.provision "shell", inline: <<-SHELL
      set +x
      apt-get update
      apt-get install -y cmake build-essential libpcre3-dev zlib1g-dev libgcrypt11-dev libicu-dev python libsqlite3-dev sqlite3 cmake-curses-gui
    SHELL
    dev.vm.provision "shell", privileged: false, inline: <<-SHELL
      set +x
      cd ~/
      tar -xjf /vagrant/cppcms.tar.bz2
      mv cppcms-* cppcms
      cd cppcms
      mkdir -p build
      cd build
      cmake -DCMAKE_INSTALL_PREFIX=/usr ..
      CPU_COUNT=$(lscpu -p | egrep -v '^#' | wc -l)
      THREADS=$(($CPU_COUNT * 2))
      make -j $THREADS
  
      cd ~/
      tar -xjf /vagrant/cppdb.tar.bz2
      mv cppdb-* cppdb
      cd cppdb
      mkdir -p build
      cd build
      cmake -DCMAKE_INSTALL_PREFIX=/usr -DSQLITE_BACKEND_INTERNAL=ON -DDISABLE_PQ=ON -DDISABLE_MYSQL=ON -DDISABLE_ODBC=ON ..
      make -j $THREADS
  
      cd ~/
      tar -xzf /vagrant/libsodium.tar.gz
      mv libsodium* libsodium
      cd libsodium
      ./configure --prefix /usr
      make -j $THREADS
    SHELL
    dev.vm.provision "shell", inline: <<-SHELL
      set +x
      cd /home/ubuntu/cppcms/build
      make install
      cd /home/ubuntu/cppdb/build
      make install
      cd /home/ubuntu/libsodium
      make install
    SHELL
  end

end
