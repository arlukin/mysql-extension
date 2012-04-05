<?php

	include('Include.php');

	//

	echo('START<br>');
	echo('Test of the UDF functions getLanguage/setLanguage<br><br>');

	verifySqlResult
	(
		'SELECT getLanguage(setLanguage(NULL, "[LANG=UK_]Hello world.[/LANG]", NULL, "SE_"), NULL, NULL, 3) as foLanguageColumn',
		'[LANG=UK_]Hello world.[/LANG]'
	);

	// Test with database table
	verifySqlResult('DROP TABLE IF EXISTS foLanguageTest', '1');
	verifySqlResult('CREATE TABLE foLanguageTest (id int NOT NULL AUTO_INCREMENT, `description` text NULL, PRIMARY KEY (id))', '1');

	verifySqlResult('TRUNCATE foLanguageTest', '1');

	verifySqlResult('INSERT into foLanguageTest (id, description) values(1, setLanguage(NULL, "Sverige", "SE_", ""))', '1');
	verifySqlResult('UPDATE foLanguageTest SET description = setLanguage(description, "England", "UK_", "FI_") WHERE id = 1', '1');
	verifySqlResult('UPDATE foLanguageTest SET description = setLanguage(description, "Finland", "FI_", "UK_") WHERE id = 1', '1');
	verifySqlResult('UPDATE foLanguageTest SET description = setLanguage(description, "test",    "FR_", "") WHERE id = 1', '1');
	verifySqlResult('UPDATE foLanguageTest SET description = setLanguage(description, "France",  "FR_", "FI_") WHERE id = 1', '1');
	verifySqlResult('UPDATE foLanguageTest SET description = setLanguage(description, "American. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. American", "US_", "") WHERE id = 1', '1');
	verifySqlResult('UPDATE foLanguageTest SET description = setLanguage(description, "", "SE_", "") WHERE id = 1', '1');

	// FI_ are now the default language.

	// Sets the language column with the foLanguageTag, UK_ has an invalid tag, will not be updated.
	verifySqlResult
	(
		'UPDATE foLanguageTest SET description = setLanguage(description, "asdfasdf[LANG=UK_]England 2[/LADDNG][LANG=FI_]Finland 2[/LANG][LANG=SE_]Sverige 2[/LANG]
asdf
[LANG=FR_]France 2[/LANG]asdfasdf", NULL, "FR_") WHERE id = 1',
		'1'
	);


	// FR_ are now the default language.
	verifySqlResult('SELECT getLanguage(description, "SE_", "UK_", 0) AS Sverige2 FROM foLanguageTest',  'Sverige 2');
	verifySqlResult('SELECT getLanguage(description, "FI_", "UK_", 1) AS Finland2 FROM foLanguageTest',  'Finland 2');
	verifySqlResult('SELECT getLanguage(description, "FR_", "UK_", 1) AS France2 FROM foLanguageTest',   'France 2');
	verifySqlResult('SELECT getLanguage(description, "IS_", "UK_", 1) AS England FROM foLanguageTest',   'England');
	verifySqlResult('SELECT getLanguage(description, "IS_", "MI_", 1) AS France2 FROM foLanguageTest	', 'France 2');
	verifySqlResult('SELECT getLanguage(description, "US_", "UK_", 1) AS AmericanThisIsALongText FROM foLanguageTest', 'American. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. American');

	verifySqlResult('SELECT getLanguage(description, "", "",     	0) AS FullTag FROM foLanguageTest', '[LANG=FR_]France 2[/LANG][LANG=UK_]England[/LANG][LANG=US_]American. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. American[/LANG][LANG=FI_]Finland 2[/LANG][LANG=SE_]Sverige 2[/LANG]');
	verifySqlResult('SELECT getLanguage(description, "", "",     	1) AS FullTag FROM foLanguageTest', '[LANG=FR_]France 2[/LANG][LANG=UK_]England[/LANG][LANG=US_]American. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. American[/LANG][LANG=FI_]Finland 2[/LANG][LANG=SE_]Sverige 2[/LANG]');
	verifySqlResult('SELECT getLanguage(description, "", "",     	2) AS FullTag FROM foLanguageTest', '[LANG=FR_]France 2[/LANG][LANG=UK_]England[/LANG][LANG=US_]American. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. American[/LANG][LANG=FI_]Finland 2[/LANG][LANG=SE_]Sverige 2[/LANG]');
	verifySqlResult('SELECT getLanguage(description, "", "",     	3) AS FullTag FROM foLanguageTest', '[LANG=FR_]France 2[/LANG][LANG=UK_]England[/LANG][LANG=US_]American. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. American[/LANG][LANG=FI_]Finland 2[/LANG][LANG=SE_]Sverige 2[/LANG]');

	verifySqlResult('SELECT getLanguage(description, "SE_", NULL, 	0) AS Sverige2 FROM foLanguageTest', 'Sverige 2');
	verifySqlResult('SELECT getLanguage(description, "SE_", NULL, 	1) AS Sverige2 FROM foLanguageTest', 'Sverige 2');
	verifySqlResult('SELECT getLanguage(description, "SE_", NULL, 	2) AS Sverige2 FROM foLanguageTest', 'Sverige 2');
	verifySqlResult('SELECT getLanguage(description, "SE_", NULL, 	3) AS Sverige2 FROM foLanguageTest', 'Sverige 2');

	verifySqlResult('SELECT getLanguage(description, "SE_", "UK_", 	0) AS Sverige2 FROM foLanguageTest', 'Sverige 2');
	verifySqlResult('SELECT getLanguage(description, "SE_", "UK_", 	1) AS Sverige2 FROM foLanguageTest', 'Sverige 2');
	verifySqlResult('SELECT getLanguage(description, "SE_", "UK_", 	2) AS Sverige2 FROM foLanguageTest', 'Sverige 2');
	verifySqlResult('SELECT getLanguage(description, "SE_", "UK_", 	3) AS Sverige2 FROM foLanguageTest', 'Sverige 2');

	verifySqlResult('SELECT getLanguage(description, NULL, "UK_", 	0) AS FullTag FROM foLanguageTest', '[LANG=FR_]France 2[/LANG][LANG=UK_]England[/LANG][LANG=US_]American. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. American[/LANG][LANG=FI_]Finland 2[/LANG][LANG=SE_]Sverige 2[/LANG]');
	verifySqlResult('SELECT getLanguage(description, NULL, "UK_", 	1) AS FullTag FROM foLanguageTest', '[LANG=FR_]France 2[/LANG][LANG=UK_]England[/LANG][LANG=US_]American. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. American[/LANG][LANG=FI_]Finland 2[/LANG][LANG=SE_]Sverige 2[/LANG]');
	verifySqlResult('SELECT getLanguage(description, NULL, "UK_", 	2) AS FullTag FROM foLanguageTest', '[LANG=FR_]France 2[/LANG][LANG=UK_]England[/LANG][LANG=US_]American. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. American[/LANG][LANG=FI_]Finland 2[/LANG][LANG=SE_]Sverige 2[/LANG]');
	verifySqlResult('SELECT getLanguage(description, NULL, "UK_", 	3) AS FullTag FROM foLanguageTest', '[LANG=FR_]France 2[/LANG][LANG=UK_]England[/LANG][LANG=US_]American. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. American[/LANG][LANG=FI_]Finland 2[/LANG][LANG=SE_]Sverige 2[/LANG]');

	verifySqlResult('SELECT getLanguage(description, "NON", NULL, 	0) AS empty FROM foLanguageTest',     '');
	verifySqlResult('SELECT getLanguage(description, "NON", NULL, 	1) AS France2 FROM foLanguageTest',   'France 2');
	verifySqlResult('SELECT getLanguage(description, "NON", NULL, 	2) AS empty FROM foLanguageTest',     '');
	verifySqlResult('SELECT getLanguage(description, "NON", NULL, 	3) AS SimpleTag FROM foLanguageTest', '[LANG=NON][/LANG][LANG=FR_]France 2[/LANG]');

	verifySqlResult('SELECT getLanguage(description, "NON", "UK_", 	0) AS England FROM foLanguageTest', 'England');
	verifySqlResult('SELECT getLanguage(description, "NON", "UK_", 	1) AS England FROM foLanguageTest', 'England');
	verifySqlResult('SELECT getLanguage(description, "NON", "UK_", 	2) AS SimpleTagEngland FROM foLanguageTest', '[LANG=NON][/LANG][LANG=UK_]England[/LANG]');
	verifySqlResult('SELECT getLanguage(description, "NON", "UK_", 	3) AS SimpleTagEngland FROM foLanguageTest', '[LANG=NON][/LANG][LANG=UK_]England[/LANG]');

	verifySqlResult('SELECT getLanguage(description, "NON", "NON", 	0) AS empty FROM foLanguageTest', '');
	verifySqlResult('SELECT getLanguage(description, "NON", "NON", 	1) AS France2 FROM foLanguageTest', 'France 2');
	verifySqlResult('SELECT getLanguage(description, "NON", "NON", 	2) AS empty FROM foLanguageTest', '');
	verifySqlResult('SELECT getLanguage(description, "NON", "NON", 	3) AS SimpleTagFrance2 FROM foLanguageTest', '[LANG=NON][/LANG][LANG=FR_]France 2[/LANG]');

	// Tests new line "\n"
	verifySqlResult('INSERT into foLanguageTest (description) values(setLanguage(NULL, "England\neng", "UK", ""))', '1');
	verifySqlResult('SELECT getLanguage(description, "UK", "UK", "N") as description  FROM foLanguageTest WHERE id = 2', "England\neng");


	// Translate a regulare column to a foLanguageColumn.
	verifySqlResult('INSERT into foLanguageTest (id, description) values(3, "1")', '1');
	verifySqlResult('INSERT into foLanguageTest (id, description) values(4, "12")', '1');
	verifySqlResult('INSERT into foLanguageTest (id, description) values(5, "123")', '1');
	verifySqlResult('INSERT into foLanguageTest (id, description) values(6, "1234")', '1');
	verifySqlResult('INSERT into foLanguageTest (id, description) values(7, "12345")', '1');

	verifySqlResult('UPDATE foLanguageTest SET description = setLanguage(NULL, description, "UK_", "UK_") WHERE id in (3, 4, 5, 6, 7)', '1');

	verifySqlResult('SELECT getLanguage(description, NULL, "UK_", 	3) FROM foLanguageTest WHERE id in (3)', '[LANG=UK_]1[/LANG]');
	verifySqlResult('SELECT getLanguage(description, NULL, "UK_", 	3) FROM foLanguageTest WHERE id in (4)', '[LANG=UK_]12[/LANG]');
	verifySqlResult('SELECT getLanguage(description, NULL, "UK_", 	3) FROM foLanguageTest WHERE id in (5)', '[LANG=UK_]123[/LANG]');
	verifySqlResult('SELECT getLanguage(description, NULL, "UK_", 	3) FROM foLanguageTest WHERE id in (6)', '[LANG=UK_]1234[/LANG]');
	verifySqlResult('SELECT getLanguage(description, NULL, "UK_", 	3) FROM foLanguageTest WHERE id in (7)', '[LANG=UK_]12345[/LANG]');

	writeStat();

	// How fast is the function
	echo('Benchmark test 10000000 expected time 16.41<br>');
	$startTime = getMicrotime();

	executeOnDbReturnOneColumn('SELECT BENCHMARK(10000000,getLanguage(setLanguage("", "England", "UK", "FI"), "UK", NULL, 0))');


	echo('Time: ' . (getMicrotime()-$startTime) .'<br>');

	echo('FINISHED<br>');

?>