#
# Configure mysql to use the UDF in FoFunctions.so
#

use mysql;
#DROP function calcPrice;
#DROP function calcCost;
#DROP function validateMinMaxDate;

CREATE FUNCTION calcPrice RETURNS REAL SONAME "FoFunctions.so";
CREATE FUNCTION calcCost RETURNS REAL SONAME "FoFunctions.so";
CREATE FUNCTION validateMinMaxDate RETURNS REAL SONAME "FoFunctions.so";
