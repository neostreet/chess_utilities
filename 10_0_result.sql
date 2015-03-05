use chess

select count(*),result
from blitz_games
where time_control = '10|0'
group by 2
order by 2 desc;

select count(*)
from blitz_games
where time_control = '10|0';

quit
