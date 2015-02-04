use chess

select my_elo_after,result,blitz_game_date,id
from blitz_games
order by my_elo_after,id desc;

quit
