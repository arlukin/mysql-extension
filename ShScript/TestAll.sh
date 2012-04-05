#
# Test the FoLanguage.so functions.
#
HM=`date +%H-%M`

mysql -uroot -p83krypandegurkor foMaster < ../Sql/TestLanguage.sql
mysql -uroot -p83krypandegurkor foMaster < ../Sql/TestCalc.sql
mysql -uroot -p83krypandegurkor foMaster < ../Sql/TestValidateMinMax.sql

tail /var/lib/mysql/sql1.err
echo "Finished " . HM > ../Log/TestAll.log