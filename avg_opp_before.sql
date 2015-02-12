use chess

select sum(opponent_elo_before) / count(*)
from blitz_games
where opponent_elo_before is not null;

quit
