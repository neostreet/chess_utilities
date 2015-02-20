use chess

select my_elo_delta,my_elo_after,result,blitz_game_date
from blitz_games
order by 1 desc,id desc;

quit
