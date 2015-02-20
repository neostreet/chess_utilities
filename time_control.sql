use chess

select count(*),time_control
from blitz_games
group by time_control
order by 1 desc,2;

quit
