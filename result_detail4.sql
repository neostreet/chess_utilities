use chess

select result,my_elo_after,opponent_name,blitz_game_date
from blitz_games
order by id desc;

quit
