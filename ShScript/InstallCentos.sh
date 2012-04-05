#
# Install all mysql functions on Centos.
#

# Required for compilation

yum -y install mysql-devel
yum -y install gcc-c++

# Clean up old files

rm ../Bin/*

# Compilation

gcc  -Wall -fPIC -I /usr/include/mysql -shared -lstdc++ -o ../Bin/FoFunctions.so ../FoFunctions.cpp
gcc  -Wall -fPIC -I /usr/include/mysql -shared -lstdc++ -o ../Bin/FoLanguage.so  ../FoLanguage.cpp


sudo /etc/init.d/mysqld stop
cp ../Bin/FoFunctions.so /var/lib/mysql/plugins/FoFunctions.so
cp ../Bin/FoLanguage.so  /var/lib/mysql/plugins/FoLanguage.so
sudo /etc/init.d/mysqld start

mysql -uroot -p < ../Sql/InstallFoFunctions.sql
mysql -uroot -p < ../Sql/InstallFoLanguage.sql
echo "Finished"