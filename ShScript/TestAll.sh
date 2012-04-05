#
# Test the FoLanguage.so functions.
#
HM=`date +%H-%M`

mysql --execute "CREATE DATABASE foTest"
mysql -uroot -p foTest < ../Sql/TestLanguage.sql
mysql -uroot -p foTest < ../Sql/TestCalc.sql
mysql -uroot -p foTest < ../Sql/TestValidateMinMax.sql
mysql --execute "DROP DATABASE foTest"

tail /var/log/mysqld.log
echo "Finished"