#
# File used when testing and debugging the mysql functions.
# It will automatically be run in some of the compile scripts.
#

use mysql;



#//DROP function calcPrice;
#//DROP function calcCost;
#//DROP function validateMinMaxDate;

DROP function getLanguage;
DROP function setLanguage;

#//CREATE FUNCTION calcPrice RETURNS REAL SONAME "fo_functions.so";
#//CREATE FUNCTION calcCost RETURNS REAL SONAME "fo_functions.so";
#//CREATE FUNCTION validateMinMaxDate RETURNS REAL SONAME "fo_functions.so";

CREATE FUNCTION getLanguage RETURNS STRING SONAME "fo_functions.so";
CREATE FUNCTION setLanguage RETURNS STRING SONAME "fo_functions.so";

use foMaster;
#select setLanguage(NULL, "Moi mukulat]", "FI_", "UK_") as moo;

# truncate daniel;
# insert into daniel (id, description) values(1, setLanguage(NULL, "Sverige", "SE_", ''));
# select * from daniel;
# update daniel set description = setLanguage(description, "England", "UK_", "FI_") where id = 1 ;
# select * from daniel;

# NEW TEST

# Test with database table
DROP TABLE foLanguageTest;
CREATE TABLE foLanguageTest (id int NOT NULL AUTO_INCREMENT, `description` text NULL, PRIMARY KEY (id));

TRUNCATE foLanguageTest;

	INSERT into foLanguageTest (id, description) values(3, setLanguage(NULL, "1", "SE_", ''));
	UPDATE foLanguageTest SET description = setLanguage(NULL, description, "UK_", "UK_") WHERE id in (3, 4, 5, 6, 7);

	SELECT id, getLanguage(description, NULL, "UK_", 	3) FROM foLanguageTest WHERE id in (3, 4, 5, 6, 7);

