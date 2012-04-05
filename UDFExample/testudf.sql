CREATE FUNCTION myfunc_int RETURNS INTEGER SONAME "udf_example.so";

DROP FUNCTION metaphon;
DROP FUNCTION myfunc_double;
DROP FUNCTION myfunc_int;
DROP FUNCTION sequence;
DROP FUNCTION lookup;
DROP FUNCTION reverse_lookup;
DROP FUNCTION avgcost;








CREATE FUNCTION metaphon RETURNS STRING SONAME "udf_example.so";
CREATE FUNCTION myfunc_double RETURNS REAL SONAME "udf_example.so";



CREATE FUNCTION sequence RETURNS INTEGER SONAME "udf_example.so";
CREATE FUNCTION lookup RETURNS STRING SONAME "udf_example.so";
CREATE FUNCTION reverse_lookup RETURNS STRING SONAME "udf_example.so";
CREATE AGGREGATE FUNCTION avgcost RETURNS REAL SONAME "udf_example.so";

