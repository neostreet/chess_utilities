use chess

select sum(my_elo_delta),count(*),time_control
from blitz_games
group by 3
order by 1 desc,3;

select sum(my_elo_delta),count(*)
from blitz_games;

quit
