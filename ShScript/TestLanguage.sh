#
# Test the FoLanguage.so functions.
#

mysql -uroot -p foMaster < ../Sql/TestLanguage.sql

tail /var/lib/mysql/sql1.err
echo "Finished"