# Compile SUSE

gcc -Wall -fPIC -I /usr/src/packages/BUILD/mysql-4.1.20/include/  -shared -lstdc++ -o ../Bin/FoFunctions.so ../FoFunctions.cpp 
gcc -Wall -fPIC -I /usr/src/packages/BUILD/mysql-4.1.20/include/  -shared -lstdc++ -o ../Bin/FoLanguage.so ../FoLanguage.cpp 

/etc/init.d/mysql stop
rm /var/lib/mysql/sql1.err
cp ../Bin/FoFunctions.so /usr/lib64/fo_functions.so
cp ../Bin/FoLanguage.so /usr/lib64/FoLanguage.so
/etc/init.d/mysql start

mysql -uroot -p foMaster < ../Sql/Test.sql

vi /var/lib/mysql/sql1.err 
