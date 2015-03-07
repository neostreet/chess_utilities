use chess

select sum(my_elo_delta),count(*),blitz_game_date
from blitz_games
group by 3
order by 1 desc,3 desc;

quit
