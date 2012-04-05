# Compile REDHAT

sudo gcc  -Wall -pg -O0 -fPIC -I /usr/local/mysql4/include/mysql/ -shared -lstdc++ -o ../Bin/FoLanguage.so ../FoLanguage.cpp

sudo /etc/init.d/mysqld4 stop
sudo rm  /var/lib/mysql4/fo-nsg-db1.err
sudo cp ../Bin/FoLanguage.so  /usr/local/mysql4/lib/mysql/FoLanguage.so
sudo /etc/init.d/mysqld4 start

/usr/local/mysql4/bin/mysql --defaults-file=/usr/local/mysql4/my.cnf -udali -p foMaster < ../Sql/Testsmall.sql

#sudo vi /var/lib/mysql4/fo-nsg-db1.err


