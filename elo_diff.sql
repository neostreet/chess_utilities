use chess

select my_elo_after - opponent_elo_after,my_elo_after
from blitz_games
order by id;

quit
