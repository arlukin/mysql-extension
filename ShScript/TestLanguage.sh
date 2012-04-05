#
# Test the FoLanguage.so functions.
#

mysql -uroot -p --execute "CREATE DATABASE foTest"
mysql -uroot -p foTest < ../Sql/TestLanguage.sql
mysql -uroot -p --execute "DROP DATABASE foTest"

tail /var/log/mysqld.log
echo "Finished"