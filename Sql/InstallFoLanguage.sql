#
# Configure mysql to use the UDF in FoLanguage.so
#

use mysql;

#DROP function getLanguage;
#DROP function setLanguage;

CREATE FUNCTION getLanguage RETURNS STRING SONAME "FoLanguage.so";
CREATE FUNCTION setLanguage RETURNS STRING SONAME "FoLanguage.so";
