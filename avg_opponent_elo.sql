use chess

select sum(opponent_elo_before) / count(*)
from rapid_games;

quit
