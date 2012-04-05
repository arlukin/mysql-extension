#
# Install all mysql functions on a SUSE computer
#

# Compile for SUSE
gcc -Wall -fPIC -I /usr/src/packages/BUILD/mysql-4.1.20/include/  -shared -lstdc++ -o ../Bin/FoFunctions.so ../FoFunctions.cpp 
gcc -Wall -fPIC -I /usr/src/packages/BUILD/mysql-4.1.20/include/  -shared -lstdc++ -o ../Bin/FoLanguage.so  ../FoLanguage.cpp 

/etc/init.d/mysql stop
cp ../Bin/FoFunctions.so /usr/lib64/FoFunctions.so
cp ../Bin/FoLanguage.so /usr/lib64/FoLanguage.so
/etc/init.d/mysql start

mysql -uroot -p < ../Sql/InstallFoFunctions.sql < ../Sql/InstallFoLanguage.sql
echo "Finished"
