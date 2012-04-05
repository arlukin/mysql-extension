
select calcPrice(100, NULL,				NULL, 						NULL) 		as  "100";
select calcPrice(100, "*1.5+50*4",		NULL, 						NULL) 		as  "500";
select calcPrice(100, "*1.5+50*4-40", NULL, 						NULL) 		as  "460";
select calcPrice(100, "*1.5+50*4-40", "*2.5+4-50", 			NULL) 				as  "1104";
select calcPrice(100, "*1.5+50*4-40", "*2.5+4-50", 			"+600*2.2+600-.1")  as  "4349";


-- sto-lon(+50)-ams(+10%)-jfk
select calcPrice(100, "+50*1.1",		NULL, 						NULL) 					as  "160";

-- sto-lon(+50)-ams(-10%)-jfk
select calcPrice(100, "+50*0.9",		NULL, 						NULL) 					as  "140";


select calcCost(100, 200, "*2+500*1.5", NULL, 	NULL, 					NULL) as  "1000";
select calcCost(100, 200, "*2+500*1.5", "*0.8", NULL, 					NULL) as  "800";
select calcCost(100, 200, "*2+500*1.5", "*0.8", "*1.5+200*1.2",	NULL) as  "1680";

select calcCost(100, 200, "*2+500*1.5", NULL, 	NULL, 					1) as  "750";
select calcCost(100, 200, "*2+500*1.5", "*0.8", NULL, 					1) as  "600";
select calcCost(100, 200, "*2+500*1.5", "*0.8", "*1.5+200*1.2",	1) as  "1320";


	-- Weekday out/in- outDate/inDate måste vara en av dessa dagar.
-- Om ej indatum angivits.
	select validateMinMaxDate("2002-07-03", NULL, 				"1,0,0",	"0,0,1", 	"1|2|3",			 NULL); select " ";
	select validateMinMaxDate("2002-07-03", NULL, 				NULL, 	 	NULL, 		"1|2|3",			 NULL); select " ";
	select validateMinMaxDate("2002-07-03", NULL, 				NULL, 	 	NULL, 		"1|2|3",			 NULL); select " ";	
	
	select validateMinMaxDate("2002-07-03", "2002-07-05", "0,0,0", "1,0,0", NULL,					 NULL); select " ";
	select validateMinMaxDate("2002-07-03", "2002-07-05", NULL, 	 NULL, 		NULL,					 NULL); select " ";
	select validateMinMaxDate("2002-07-03", "2002-07-05", "0,0,0", "1,0,0", "3", 					 "5"); select " ";
	select validateMinMaxDate("2002-07-07", "2002-07-08", "0,0,0", "1,0,0", "7", 					 "1"); select " ";
	select validateMinMaxDate("2002-07-07", "2002-07-08", "0,0,0", "1,0,0", "123:47", 		 "45:123"); select " ";
	select validateMinMaxDate("2002-07-01", "2002-07-07", "0,0,0", "1,0,0", "5|2|3:4|2|1", "4|5:1|2|3|7"); select " ";
	select validateMinMaxDate("2002-07-01", "2002-07-07", "0,0,0", "1,0,0", "5|1|3:4|2",   "4|7:2|3|7"); select " ";
	select validateMinMaxDate("2002-07-02", "2002-07-07", "0,0,0", "1,0,0", "5|1|3:4|2",   "4|7:2|3|7"); select " ";

	-- Min: 2d - Måste vara borta två dagar exlusive utresedatum inklusive inresedatum, ie: 2002-07-03 -- 2002-07-05..
	select validateMinMaxDate("2002-07-03", "2002-07-05", "0,0,2",			 		"1,0,0", 							"1|2|3|4|5|6|7", 		"1|2|3|4|5|6|7"); select " ";
	select validateMinMaxDate("2002-07-14", "2002-07-18", "0,0,4",			 		"1,0,0", 							"1|2|3|4|5|6|7", 		"1|2|3|4|5|6|7"); select " ";
	select validateMinMaxDate("2002-07-03", "2002-07-05", "0,0,4|0,0,2", 		"1,0,0|1,0,0", 				"1|2|3|4|5|6|7", 		"1|2|3|4|5|6|7"); select " ";

	-- Max: 2d - Får inte vara borta längre än två dagar exlusive utresedatum inklusive inresedatum. ie: 2002-07-03 -- 2002-07-05..
	-- En sektion i max får ej innehålla or |, bara slash /... ie: 0,0,2/0,0,4 ej 0,0,1|0,0,5/0,0,4
	select validateMinMaxDate("2002-07-03", "2002-07-05", "0,0,0",			 		"0,0,2", 							"1|2|3|4|5|6|7", 		"1|2|3|4|5|6|7"); select " ";
	select validateMinMaxDate("2002-07-14", "2002-07-18", "0,0,0",			 		"0,0,4", 							"1|2|3|4|5|6|7", 		"1|2|3|4|5|6|7"); select " ";
	select validateMinMaxDate("2002-07-14", "2002-07-18", "0,0,0:0,0,0", 		"0,0,2:0,0,4", 				"1|2|3|4|5|6|7", 		"1|2|3|4|5|6|7"); select " ";

	-- th/1y
	select validateMinMaxDate("2002-07-03", "2002-07-05", "4", 							"1,0,0", 							"1|2|3|4|5|6|7", 		"1|2|3|4|5|6|7"); select " ";

	-- fr-su/1y
	select validateMinMaxDate("2002-07-04", "2002-07-07", "5-7", 						"1,0,0", 							"1|2|3|4|5|6|7", 		"1|2|3|4|5|6|7"); select " ";

	-- mo,su,tu/1y
	select validateMinMaxDate("2002-07-01", "2002-07-02", "1|7|2",					"1,0,0", 							"1|2|3|4|5|6|7", 		"1|2|3|4|5|6|7"); select " ";
	select validateMinMaxDate("2002-07-06", "2002-07-07", "1|7|2",					"1,0,0", 							"1|2|3|4|5|6|7", 		"1|2|3|4|5|6|7"); select " ";

	-- fr-su, tu-we/1y
	select validateMinMaxDate("2002-07-04", "2002-07-07", "5-7|2-3",				"1,0,0", 							"1|2|3|4|5|6|7", 		"1|2|3|4|5|6|7"); select " ";
	select validateMinMaxDate("2002-07-01", "2002-07-03", "5-7|2-3",				"1,0,0", 							"1|2|3|4|5|6|7", 		"1|2|3|4|5|6|7"); select " ";

	-- fr-su, tu-we, mo, th, TU/1y
	select validateMinMaxDate("2002-07-01", "2002-07-02", "5-7|2-3|1|4|2",	"1,0,0", 							"1|2|3|4|5|6|7", 		"1|2|3|4|5|6|7"); select " ";

	-- 3d, mo, tu-we/1Y
	select validateMinMaxDate("2002-06-30", "2002-07-01", "0,0,3|1|2-3",		"1,0,0", 							"1|2|3|4|5|6|7", 		"1|2|3|4|5|6|7"); select " ";
	select validateMinMaxDate("2002-07-02", "2002-07-05", "0,0,3|1|2-3",		"1,0,0", 							"1|2|3|4|5|6|7", 		"1|2|3|4|5|6|7"); select " ";

	-- 3d+tu/1y
	select validateMinMaxDate("2002-07-01", "2002-07-04", "0,0,3&2",				"1,0,0", 							"1|2|3|4|5|6|7", 		"1|2|3|4|5|6|7"); select " ";	
	select validateMinMaxDate("2002-06-30", "2002-07-03", "0,0,3&2",				"1,0,0", 							"1|2|3|4|5|6|7", 		"1|2|3|4|5|6|7"); select " ";

	-- 7d/1m or 3d/4m
	select validateMinMaxDate("2002-07-01", "2002-07-04", "0,0,7:0,0,3",		"1,0,0:1,0,0", 				"1|2|3|4|5|6|7", 		"1|2|3|4|5|6|7"); select " ";

	-- we-we/1m
	select validateMinMaxDate("2002-07-02", "2002-07-10", "3-3",		"1,0,0", 	"1|2|3|4|5|6|7", 		"1|2|3|4|5|6|7"); select " ";


	select "FAIL";


	-- Invalid min/max

	-- Weekday
	select validateMinMaxDate("2002-07-03", "2002-07-05", "0,0,0",			 		"1,0,0", 							"2", 					"4"); select " ";
	select validateMinMaxDate("2002-07-07", "2002-07-08", "0,0,0",			 		"1,0,0", 							"7", 					"2"); select " ";
	select validateMinMaxDate("2002-07-01", "2002-07-07", "0,0,0", "1,0,0", "5|2|3:4|5|1", "4|5:1|2|3"); select " ";
	select validateMinMaxDate("2002-07-01", "2002-07-07", "0,0,0", "1,0,0", "5|1|3:4|5", "4|5:1|2|3|7"); select " ";

	-- Min
	select validateMinMaxDate("2002-07-03", "2002-07-05", "0,0,3",			 		"1,0,0", 							"1|2|3|4|5|6|7", 		"1|2|3|4|5|6|7"); select " ";
	select validateMinMaxDate("2002-07-14", "2002-07-18", "0,0,5",			 		"1,0,0", 							"1|2|3|4|5|6|7", 		"1|2|3|4|5|6|7"); select " ";
	select validateMinMaxDate("2002-07-03", "2002-07-05", "0,0,5|0,0,3", 		"1,0,0|1,0,0", 				"1|2|3|4|5|6|7", 		"1|2|3|4|5|6|7"); select " ";

	-- Max:
	select validateMinMaxDate("2002-07-03", "2002-07-05", "0,0,0",			 		"0,0,1", 							"1|2|3|4|5|6|7", 		"1|2|3|4|5|6|7"); select " ";
	select validateMinMaxDate("2002-07-14", "2002-07-18", "0,0,0",			 		"0,0,3", 							"1|2|3|4|5|6|7", 		"1|2|3|4|5|6|7"); select " ";
	select validateMinMaxDate("2002-07-14", "2002-07-18", "0,0,0/0,0,0", 		"0,0,2/0,0,3", 				"1|2|3|4|5|6|7", 		"1|2|3|4|5|6|7"); select " ";

	-- fr-su/1y
	select validateMinMaxDate("2002-07-07", "2002-07-07", "5-7", 						"1,0,0", 							"1|2|3|4|5|6|7", 		"1|2|3|4|5|6|7"); select " ";

	-- mo,su,tu/1y
	select validateMinMaxDate("2002-07-02", "2002-07-02", "1-1|7|2",				"1,0,0", 							"1|2|3|4|5|6|7", 		"1|2|3|4|5|6|7"); select " ";
	select validateMinMaxDate("2002-07-07", "2002-07-07", "1-1|7|2",				"1,0,0", 							"1|2|3|4|5|6|7", 		"1|2|3|4|5|6|7"); select " ";

	-- fr-su, tu-we/1y
	select validateMinMaxDate("2002-07-05", "2002-07-07", "5-7|2-3",				"1,0,0", 							"1|2|3|4|5|6|7", 		"1|2|3|4|5|6|7"); select " ";
	select validateMinMaxDate("2002-07-03", "2002-07-04", "5-7|2-3",				"1,0,0", 							"1|2|3|4|5|6|7", 		"1|2|3|4|5|6|7"); select " ";

	-- 3d, mo, tu-we/1Y
	select validateMinMaxDate("2002-07-01", "2002-07-02", "0,0,3|1|2-3",		"1,0,0", 							"1|2|3|4|5|6|7", 		"1|2|3|4|5|6|7"); select " ";
	select validateMinMaxDate("2002-07-02", "2002-07-04", "0,0,3|1|2-3",		"1,0,0", 							"1|2|3|4|5|6|7", 		"1|2|3|4|5|6|7"); select " ";

	-- 3d+tu/1y
	select validateMinMaxDate("2002-07-01", "2002-07-03", "0,0,3&2",				"1,0,0", 							"1|2|3|4|5|6|7", 		"1|2|3|4|5|6|7"); select " ";
	select validateMinMaxDate("2002-06-30", "2002-07-02", "0,0,3&2",				"1,0,0", 							"1|2|3|4|5|6|7", 		"1|2|3|4|5|6|7"); select " ";

	-- 7d/1m or 3d/4m
	select validateMinMaxDate("2002-07-01", "2002-07-02", "0,0,7/0,0,3",		"1,0,0/1,0,0", 				"1|2|3|4|5|6|7", 		"1|2|3|4|5|6|7"); select " ";

	-- we-we/1m
	select validateMinMaxDate("2002-07-02", "2002-07-03", "3-3",		"1,0,0", 	"1|2|3|4|5|6|7", 		"1|2|3|4|5|6|7"); select " ";

	
-- Om ej indatum angivits
	select validateMinMaxDate("2002-07-03", NULL, 				NULL, 	 NULL, 		"1|2",			 NULL); select " ";
