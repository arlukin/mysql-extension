
select calcPrice(100, NULL,    NULL,       NULL)   as  "100";
select calcPrice(100, "*1.5+50*4",  NULL,       NULL)   as  "500";
select calcPrice(100, "*1.5+50*4-40", NULL,       NULL)   as  "460";
select calcPrice(100, "*1.5+50*4-40", "*2.5+4-50",    NULL)     as  "1104";
select calcPrice(100, "*1.5+50*4-40", "*2.5+4-50",    "+600*2.2+600-.1")  as  "4349";


-- sto-lon(+50)-ams(+10%)-jfk
select calcPrice(100, "+50*1.1",  NULL,       NULL)      as  "160";

-- sto-lon(+50)-ams(-10%)-jfk
select calcPrice(100, "+50*0.9",  NULL,       NULL)      as  "140";


select calcCost(100, 200, "*2+500*1.5", NULL,  NULL,      NULL) as  "1000";
select calcCost(100, 200, "*2+500*1.5", "*0.8", NULL,      NULL) as  "800";
select calcCost(100, 200, "*2+500*1.5", "*0.8", "*1.5+200*1.2", NULL) as  "1680";

select calcCost(100, 200, "*2+500*1.5", NULL,  NULL,      1) as  "750";
select calcCost(100, 200, "*2+500*1.5", "*0.8", NULL,      1) as  "600";
select calcCost(100, 200, "*2+500*1.5", "*0.8", "*1.5+200*1.2", 1) as  "1320";




SELECT BENCHMARK(10000000,calcCost(100, 200, "*2+500*1.5", "*0.8", "*1.5+200*1.2", 1)) as 18dot98;
SELECT BENCHMARK(10000000,calcPrice(100, "+50*1.1",  NULL,       NULL)) as 6dot02;