use chess

select result,time_control,my_elo_after,opponent_name,blitz_game_date
from blitz_games
order by id;

quit
