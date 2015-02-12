use chess

select sum(opponent_elo_after) / count(*)
from blitz_games;

quit
