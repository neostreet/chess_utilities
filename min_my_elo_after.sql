use chess

select my_elo_after,blitz_game_date,id
from blitz_games
where my_elo_after = (select min(my_elo_after) from blitz_games)
order by id desc;

quit
