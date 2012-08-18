#
# Create database
#
CREATE DATABASE foTest;
use foTest;

#
# Create test table
#
DROP TABLE IF EXISTS foLanguageTest;
CREATE TABLE foLanguageTest (id int NOT NULL AUTO_INCREMENT, `description` text NULL, PRIMARY KEY (id));


#
# Test without database table
#
SELECT setLanguage(NULL, '[LANG=UK_]Hello world.[/LANG]', NULL, 'SE_') as foLanguageColumn;


#
# Test with database table
#
INSERT into foLanguageTest (id, description) values(1, setLanguage(NULL, "Sverige", "SE_", ''));
UPDATE foLanguageTest SET description = setLanguage(description, "England", "UK_", "FI_") WHERE id = 1 ;
UPDATE foLanguageTest SET description = setLanguage(description, "Finland", "FI_", "UK_") WHERE id = 1;
UPDATE foLanguageTest SET description = setLanguage(description, "test",    "FR_", "") WHERE id = 1;
UPDATE foLanguageTest SET description = setLanguage(description, "France",  "FR_", "FI_") WHERE id = 1;
UPDATE foLanguageTest SET description = setLanguage(description, "American. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. American", "US_", "") WHERE id = 1;
UPDATE foLanguageTest SET description = setLanguage(description, "", "SE_", "") WHERE id = 1;

# FI_ are now the default language.

# Sets the language column with the foLanguageTag, UK_ has an invalid tag, will not be updated.
UPDATE foLanguageTest SET description = setLanguage(description, "asdfasdf[LANG=UK_]England 2[/LADDNG][LANG=FI_]Finland 2[/LANG][LANG=SE_]Sverige 2[/LANG]
asdf
[LANG=FR_]France 2[/LANG]asdfasdf", NULL, 'FR_') WHERE id = 1;

# FR_ are now the default language.

SELECT getLanguage(description, "SE_", "UK_",	0) AS Sverige2 FROM foLanguageTest;
SELECT getLanguage(description, "FI_", "UK_", 1) AS Finland2 FROM foLanguageTest;
SELECT getLanguage(description, "FR_", "UK_", 1) AS France2 FROM foLanguageTest;
SELECT getLanguage(description, "IS_", "UK_", 1) AS England FROM foLanguageTest;
SELECT getLanguage(description, "IS_", "MI_", 1) AS France2 FROM foLanguageTest	;
SELECT getLanguage(description, "US_", "UK_", 1) AS AmericanThisIsALongText FROM foLanguageTest;

SELECT getLanguage(description, "", "",     	0) AS FullTag FROM foLanguageTest;
SELECT getLanguage(description, "", "",     	1) AS FullTag FROM foLanguageTest;
SELECT getLanguage(description, "", "",     	2) AS FullTag FROM foLanguageTest;
SELECT getLanguage(description, "", "",     	3) AS FullTag FROM foLanguageTest;

SELECT getLanguage(description, "SE_", NULL, 	0) AS Sverige2 FROM foLanguageTest;
SELECT getLanguage(description, "SE_", NULL, 	1) AS Sverige2 FROM foLanguageTest;
SELECT getLanguage(description, "SE_", NULL, 	2) AS Sverige2 FROM foLanguageTest;
SELECT getLanguage(description, "SE_", NULL, 	3) AS Sverige2 FROM foLanguageTest;

SELECT getLanguage(description, "SE_", "UK_", 	0) AS Sverige2 FROM foLanguageTest;
SELECT getLanguage(description, "SE_", "UK_", 	1) AS Sverige2 FROM foLanguageTest;
SELECT getLanguage(description, "SE_", "UK_", 	2) AS Sverige2 FROM foLanguageTest;
SELECT getLanguage(description, "SE_", "UK_", 	3) AS Sverige2 FROM foLanguageTest;

SELECT getLanguage(description, NULL, "UK_", 	0) AS FullTag FROM foLanguageTest;
SELECT getLanguage(description, NULL, "UK_", 	1) AS FullTag FROM foLanguageTest;
SELECT getLanguage(description, NULL, "UK_", 	2) AS FullTag FROM foLanguageTest;
SELECT getLanguage(description, NULL, "UK_", 	3) AS FullTag FROM foLanguageTest;

SELECT getLanguage(description, "NON", NULL, 	0) AS empty FROM foLanguageTest;
SELECT getLanguage(description, "NON", NULL, 	1) AS France2 FROM foLanguageTest;
SELECT getLanguage(description, "NON", NULL, 	2) AS empty FROM foLanguageTest;
SELECT getLanguage(description, "NON", NULL, 	3) AS SimpleTag FROM foLanguageTest;

SELECT getLanguage(description, "NON", 'UK_', 	0) AS England FROM foLanguageTest;
SELECT getLanguage(description, "NON", 'UK_', 	1) AS England FROM foLanguageTest;
SELECT getLanguage(description, "NON", 'UK_', 	2) AS SimpleTagEngland FROM foLanguageTest;
SELECT getLanguage(description, "NON", 'UK_', 	3) AS SimpleTagEngland FROM foLanguageTest;

SELECT getLanguage(description, "NON", 'NON', 	0) AS empty FROM foLanguageTest;
SELECT getLanguage(description, "NON", 'NON', 	1) AS France2 FROM foLanguageTest;
SELECT getLanguage(description, "NON", 'NON', 	2) AS empty FROM foLanguageTest;
SELECT getLanguage(description, "NON", 'NON', 	3) AS SimpleTagFrance2 FROM foLanguageTest;


#
# How fast is the function
#
# NOTE: Remember to turn of debug mode in the code
SELECT BENCHMARK(10000000,setLanguage('', "England", "UK", "FI")) as 15dot50;
SELECT BENCHMARK(10000000,getLanguage(setLanguage('', "England", "UK", "FI"), "UK", NULL, 0)) as 18dot08;


#
# Test carriage return.
#
INSERT into foLanguageTest (description) values(setLanguage(NULL, "England\neng", "UK", ''));
SELECT getLanguage(description, "UK", 'UK', 'N') as description  FROM foLanguageTest order by description;


#
# Translate a regulare column to a foLanguageColumn.
#
INSERT into foLanguageTest (id, description) values(3, "1");
INSERT into foLanguageTest (id, description) values(4, "12");
INSERT into foLanguageTest (id, description) values(5, "123");
INSERT into foLanguageTest (id, description) values(6, "1234");
INSERT into foLanguageTest (id, description) values(7, "12345");

UPDATE foLanguageTest SET description = setLanguage(NULL, description, "UK_", "UK_") WHERE id in (3, 4, 5, 6, 7);

SELECT id, getLanguage(description, NULL, "UK_", 3) FROM foLanguageTest WHERE id in (3, 4, 5, 6, 7);


#
# Drop test database.
#
DROP DATABASE foTest;
