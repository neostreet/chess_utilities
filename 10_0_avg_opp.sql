use chess

select sum(opponent_elo_after) / count(*)
from blitz_games
where time_control = '10|0';

quit
