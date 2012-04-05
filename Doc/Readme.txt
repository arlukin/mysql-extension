COMPILE AND INSTALL
	1. Run ShScript/Install.sh
	2. Copy UnitTest/FoLanguage.php to the apache/php htdocs.
	3. Run the FoLanguage.php script.

DIRECTORIES
	!HISTORY 					- Old files not needed anymore.
	Bin								- The compiled so files
	DebugVisualStudio	- A Visual Studio project, that includes the main
											files for the UDF functions, that can be used for debugging.
	Doc								- Documentation 
	ShScript					- Scripts that can be run in the linux shell
	Sql								- Files with sql scripts.
	UDFExample				- UDF example provided by mysql
	UnitTest					- Unit tests written in PHP.

NOTES:
* Both SUSE and slackware uses little endian.

TODO
* I mysql 5.1, kanske vi ska använda plugins istället för UDF?
http://dev.mysql.com/doc/refman/5.1/en/plugin-api.html

