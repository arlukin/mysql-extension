<?php
	include('Include.php');

	//

	echo('START<br>');
	echo('Test of the UDF functions calcPrice/calcCost/validateMinMaxDate<br><br>');

	verifySqlResult('select calcPrice(100, NULL,						NULL, 				NULL)', 							'100');
	verifySqlResult('select calcPrice(100, "*1.5+50*4",			NULL, 				NULL)', 							'500');
	verifySqlResult('select calcPrice(100, "*1.5+50*4-40", 	NULL, 				NULL)', 							'460');
	verifySqlResult('select calcPrice(100, "*1.5+50*4-40", 	"*2.5+4-50", 	NULL)', 							'1104');
	verifySqlResult('select calcPrice(100, "*1.5+50*4-40", 	"*2.5+4-50", 	"+600*2.2+600-.1")', 	'4349');


	// sto-lon(+50)-ams(+10%)-jfk
	verifySqlResult('select calcPrice(100, "+50*1.1", NULL,	NULL)', '160');

	//  sto-lon(+50)-ams(-10%)-jfk
	verifySqlResult('select calcPrice(100, "+50*0.9", NULL,	NULL)', '140');


	verifySqlResult('select calcCost(100, 200, "*2+500*1.5", NULL, 		NULL, 					NULL)', '1000');
	verifySqlResult('select calcCost(100, 200, "*2+500*1.5", "*0.8", 	NULL, 					NULL)', '800');
	verifySqlResult('select calcCost(100, 200, "*2+500*1.5", "*0.8", 	"*1.5+200*1.2",	NULL)', '1680');

	verifySqlResult('select calcCost(100, 200, "*2+500*1.5", NULL, 		NULL, 					1)', '750');
	verifySqlResult('select calcCost(100, 200, "*2+500*1.5", "*0.8",	NULL, 					1)', '600');
	verifySqlResult('select calcCost(100, 200, "*2+500*1.5", "*0.8", 	"*1.5+200*1.2",	1)', '1320');

	//-- Weekday out/in- outDate/inDate måste vara en av dessa dagar.
	//-- Om ej indatum angivits.
	verifySqlResult('select validateMinMaxDate("2002-07-03", NULL, 				"1,0,0",	"0,0,1", 	"1|2|3",			 NULL)', '0');
	verifySqlResult('select validateMinMaxDate("2002-07-03", NULL, 				NULL, 	 	NULL, 		"1|2|3",			 NULL)', '0');
	verifySqlResult('select validateMinMaxDate("2002-07-03", NULL, 				NULL, 	 	NULL, 		"1|2|3",			 NULL)', '0');

	verifySqlResult('select validateMinMaxDate("2002-07-03", "2002-07-05", "0,0,0", "1,0,0", NULL,					 NULL)', '0');
	verifySqlResult('select validateMinMaxDate("2002-07-03", "2002-07-05", NULL, 	 NULL, 		NULL,					 	 NULL)', '0');
	verifySqlResult('select validateMinMaxDate("2002-07-03", "2002-07-05", "0,0,0", "1,0,0", "3", 					 "5")',  '0');
	verifySqlResult('select validateMinMaxDate("2002-07-07", "2002-07-08", "0,0,0", "1,0,0", "7", 					 "1")',  '0');
	verifySqlResult('select validateMinMaxDate("2002-07-07", "2002-07-08", "0,0,0", "1,0,0", "123:47", 		 "45:123")', '0');
	verifySqlResult('select validateMinMaxDate("2002-07-01", "2002-07-07", "0,0,0", "1,0,0", "5|2|3:4|2|1", "4|5:1|2|3|7")', '0');
	verifySqlResult('select validateMinMaxDate("2002-07-01", "2002-07-07", "0,0,0", "1,0,0", "5|1|3:4|2",   "4|7:2|3|7")', '0');
	verifySqlResult('select validateMinMaxDate("2002-07-02", "2002-07-07", "0,0,0", "1,0,0", "5|1|3:4|2",   "4|7:2|3|7")', '0');

	// -- Min: 2d - Måste vara borta två dagar exlusive utresedatum inklusive inresedatum, ie: 2002-07-03 -- 2002-07-05..
	verifySqlResult('select validateMinMaxDate("2002-07-03", "2002-07-05", "0,0,2",			 		"1,0,0", 							"1|2|3|4|5|6|7", 		"1|2|3|4|5|6|7")', '0');
	verifySqlResult('select validateMinMaxDate("2002-07-14", "2002-07-18", "0,0,4",			 		"1,0,0", 							"1|2|3|4|5|6|7", 		"1|2|3|4|5|6|7")', '0');
	verifySqlResult('select validateMinMaxDate("2002-07-03", "2002-07-05", "0,0,4|0,0,2", 		"1,0,0|1,0,0", 				"1|2|3|4|5|6|7", 		"1|2|3|4|5|6|7")', '0');

	// -- Max: 2d - Får inte vara borta längre än två dagar exlusive utresedatum inklusive inresedatum. ie: 2002-07-03 -- 2002-07-05..
	// -- En sektion i max får ej innehålla or |, bara slash /... ie: 0,0,2/0,0,4 ej 0,0,1|0,0,5/0,0,4
	verifySqlResult('select validateMinMaxDate("2002-07-03", "2002-07-05", "0,0,0",			 		"0,0,2", 							"1|2|3|4|5|6|7", 		"1|2|3|4|5|6|7")', '0');
	verifySqlResult('select validateMinMaxDate("2002-07-14", "2002-07-18", "0,0,0",			 		"0,0,4", 							"1|2|3|4|5|6|7", 		"1|2|3|4|5|6|7")', '0');
	verifySqlResult('select validateMinMaxDate("2002-07-14", "2002-07-18", "0,0,0:0,0,0", 		"0,0,2:0,0,4", 				"1|2|3|4|5|6|7", 		"1|2|3|4|5|6|7")', '0');

	// -- th/1y
	verifySqlResult('select validateMinMaxDate("2002-07-03", "2002-07-05", "4", 							"1,0,0", 							"1|2|3|4|5|6|7", 		"1|2|3|4|5|6|7")', '0');

	// -- fr-su/1y
	verifySqlResult('select validateMinMaxDate("2002-07-04", "2002-07-07", "5-7", 						"1,0,0", 							"1|2|3|4|5|6|7", 		"1|2|3|4|5|6|7")', '0');

	// -- mo,su,tu/1y
	verifySqlResult('select validateMinMaxDate("2002-07-01", "2002-07-02", "1|7|2",					"1,0,0", 							"1|2|3|4|5|6|7", 		"1|2|3|4|5|6|7")', '0');
	verifySqlResult('select validateMinMaxDate("2002-07-06", "2002-07-07", "1|7|2",					"1,0,0", 							"1|2|3|4|5|6|7", 		"1|2|3|4|5|6|7")', '0');

	// -- fr-su, tu-we/1y
	verifySqlResult('select validateMinMaxDate("2002-07-04", "2002-07-07", "5-7|2-3",				"1,0,0", 							"1|2|3|4|5|6|7", 		"1|2|3|4|5|6|7")', '0');
	verifySqlResult('select validateMinMaxDate("2002-07-01", "2002-07-03", "5-7|2-3",				"1,0,0", 							"1|2|3|4|5|6|7", 		"1|2|3|4|5|6|7")', '0');

	// -- fr-su, tu-we, mo, th, TU/1y
	verifySqlResult('select validateMinMaxDate("2002-07-01", "2002-07-02", "5-7|2-3|1|4|2",	"1,0,0", 							"1|2|3|4|5|6|7", 		"1|2|3|4|5|6|7")', '0');

	// -- 3d, mo, tu-we/1Y
	verifySqlResult('select validateMinMaxDate("2002-06-30", "2002-07-01", "0,0,3|1|2-3",		"1,0,0", 							"1|2|3|4|5|6|7", 		"1|2|3|4|5|6|7")', '0');
	verifySqlResult('select validateMinMaxDate("2002-07-02", "2002-07-05", "0,0,3|1|2-3",		"1,0,0", 							"1|2|3|4|5|6|7", 		"1|2|3|4|5|6|7")', '0');

	// -- 3d+tu/1y
	verifySqlResult('select validateMinMaxDate("2002-07-01", "2002-07-04", "0,0,3&2",				"1,0,0", 							"1|2|3|4|5|6|7", 		"1|2|3|4|5|6|7")', '0');
	verifySqlResult('select validateMinMaxDate("2002-06-30", "2002-07-03", "0,0,3&2",				"1,0,0", 							"1|2|3|4|5|6|7", 		"1|2|3|4|5|6|7")', '0');

	// -- 7d/1m or 3d/4m
	verifySqlResult('select validateMinMaxDate("2002-07-01", "2002-07-04", "0,0,7:0,0,3",		"1,0,0:1,0,0", 				"1|2|3|4|5|6|7", 		"1|2|3|4|5|6|7")', '0');

	// -- we-we/1m
	verifySqlResult('select validateMinMaxDate("2002-07-02", "2002-07-10", "3-3",		"1,0,0", 	"1|2|3|4|5|6|7", 		"1|2|3|4|5|6|7")', '0');


	// Those will fail


	// -- Invalid min/max

	// -- Weekday
	verifySqlResult('select validateMinMaxDate("2002-07-03", "2002-07-05", "0,0,0",			 		"1,0,0", 							"2", 					"4")', '105');
	verifySqlResult('select validateMinMaxDate("2002-07-07", "2002-07-08", "0,0,0",			 		"1,0,0", 							"7", 					"2")', '105');
	verifySqlResult('select validateMinMaxDate("2002-07-01", "2002-07-07", "0,0,0", "1,0,0", "5|2|3:4|5|1", "4|5:1|2|3")', '105');
	verifySqlResult('select validateMinMaxDate("2002-07-01", "2002-07-07", "0,0,0", "1,0,0", "5|1|3:4|5", "4|5:1|2|3|7")', '105');

	// -- Min
	verifySqlResult('select validateMinMaxDate("2002-07-03", "2002-07-05", "0,0,3",			 		"1,0,0", 							"1|2|3|4|5|6|7", 		"1|2|3|4|5|6|7")', 		'106');
	verifySqlResult('select validateMinMaxDate("2002-07-14", "2002-07-18", "0,0,5",			 		"1,0,0", 							"1|2|3|4|5|6|7", 		"1|2|3|4|5|6|7")', 		'106');
	verifySqlResult('select validateMinMaxDate("2002-07-03", "2002-07-05", "0,0,5|0,0,3", 		"1,0,0|1,0,0", 				"1|2|3|4|5|6|7", 		"1|2|3|4|5|6|7")', 	'106');

	// -- Max:
	verifySqlResult('select validateMinMaxDate("2002-07-03", "2002-07-05", "0,0,0",			 		"0,0,1", 							"1|2|3|4|5|6|7", 		"1|2|3|4|5|6|7")', 		'106');
	verifySqlResult('select validateMinMaxDate("2002-07-14", "2002-07-18", "0,0,0",			 		"0,0,3", 							"1|2|3|4|5|6|7", 		"1|2|3|4|5|6|7")', 		'106');
	verifySqlResult('select validateMinMaxDate("2002-07-14", "2002-07-18", "0,0,0/0,0,0", 		"0,0,2/0,0,3", 				"1|2|3|4|5|6|7", 		"1|2|3|4|5|6|7")', 	'106');

	// -- fr-su/1y
	verifySqlResult('select validateMinMaxDate("2002-07-07", "2002-07-07", "5-7", 						"1,0,0", 							"1|2|3|4|5|6|7", 		"1|2|3|4|5|6|7")', '106');

	// -- mo,su,tu/1y
	verifySqlResult('select validateMinMaxDate("2002-07-02", "2002-07-02", "1-1|7|2",				"1,0,0", 							"1|2|3|4|5|6|7", 		"1|2|3|4|5|6|7")', '106');
	verifySqlResult('select validateMinMaxDate("2002-07-07", "2002-07-07", "1-1|7|2",				"1,0,0", 							"1|2|3|4|5|6|7", 		"1|2|3|4|5|6|7")', '106');

	// -- fr-su, tu-we/1y
	verifySqlResult('select validateMinMaxDate("2002-07-05", "2002-07-07", "5-7|2-3",				"1,0,0", 							"1|2|3|4|5|6|7", 		"1|2|3|4|5|6|7")', '106');
	verifySqlResult('select validateMinMaxDate("2002-07-03", "2002-07-04", "5-7|2-3",				"1,0,0", 							"1|2|3|4|5|6|7", 		"1|2|3|4|5|6|7")', '106');

	// -- 3d, mo, tu-we/1Y
	verifySqlResult('select validateMinMaxDate("2002-07-01", "2002-07-02", "0,0,3|1|2-3",		"1,0,0", 							"1|2|3|4|5|6|7", 		"1|2|3|4|5|6|7")', '106');
	verifySqlResult('select validateMinMaxDate("2002-07-02", "2002-07-04", "0,0,3|1|2-3",		"1,0,0", 							"1|2|3|4|5|6|7", 		"1|2|3|4|5|6|7")', '106');

	// -- 3d+tu/1y
	verifySqlResult('select validateMinMaxDate("2002-07-01", "2002-07-03", "0,0,3&2",				"1,0,0", 							"1|2|3|4|5|6|7", 		"1|2|3|4|5|6|7")', '106');
	verifySqlResult('select validateMinMaxDate("2002-06-30", "2002-07-02", "0,0,3&2",				"1,0,0", 							"1|2|3|4|5|6|7", 		"1|2|3|4|5|6|7")', '106');

	// 	verifySqlResult('-- 7d/1m or 3d/4m
		verifySqlResult('select validateMinMaxDate("2002-07-01", "2002-07-02", "0,0,7/0,0,3",		"1,0,0/1,0,0", 				"1|2|3|4|5|6|7", 		"1|2|3|4|5|6|7")', '106');

	// -- we-we/1m
	verifySqlResult('select validateMinMaxDate("2002-07-02", "2002-07-03", "3-3",		"1,0,0", 	"1|2|3|4|5|6|7", 		"1|2|3|4|5|6|7")', '106');


	//-- Om ej indatum angivits
	verifySqlResult('select validateMinMaxDate("2002-07-03", NULL, 				NULL, 	 NULL, 		"1|2",			 NULL)', '105');


	writeStat();
	echo('FINISHED<br>');

?>