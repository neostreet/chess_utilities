use chess

select result_decimal,count(*)
from blitz_games
where time_control = '10|0'
group by result_decimal
order by result_decimal desc;

select count(*)
from blitz_games
where time_control = '10|0';

quit
