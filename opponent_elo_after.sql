use chess

select opponent_elo_after,opponent_name,result,blitz_game_date
from blitz_games
order by id;

quit
