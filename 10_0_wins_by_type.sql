use chess

select result_detail,count(*)
from blitz_games
where result = 'W' and time_control = '10|0'
group by 1
order by 2 desc,1;

select count(*)
from blitz_games
where result = 'W' and time_control = '10|0';

quit
