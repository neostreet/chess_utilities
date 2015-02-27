use chess

select sum(opponent_elo_after) / count(*),count(*),blitz_game_date
from blitz_games
group by blitz_game_date
order by blitz_game_date desc;

quit
