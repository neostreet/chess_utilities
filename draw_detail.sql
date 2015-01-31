use chess

select opponent_elo_after,my_elo_after,blitz_game_date
from blitz_games
where result = 'D'
order by id;

quit
