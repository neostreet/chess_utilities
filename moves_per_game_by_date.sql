use chess

select sum(num_moves) / count(*),count(*),blitz_game_date
from blitz_games
group by blitz_game_date
order by blitz_game_date;

quit
