use chess

select count(*),time_control
from blitz_games
group by 2
order by 1 desc,2;

select count(*)
from blitz_games;

quit
