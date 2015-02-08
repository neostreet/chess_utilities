use chess

select my_elo_delta,my_elo_after,blitz_game_date
from blitz_games
where result = 'D'
order by id;

quit
