use chess

select result,count(*)
from blitz_games
where color = 'B' and time_control = '10|0'
group by result
order by result desc;

select count(*)
from blitz_games
where color = 'B' and time_control = '10|0';

quit
