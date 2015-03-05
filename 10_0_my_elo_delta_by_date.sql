use chess

select sum(my_elo_delta),count(*),blitz_game_date
from blitz_games
where time_control = '10|0'
group by 3
order by 3;

quit
