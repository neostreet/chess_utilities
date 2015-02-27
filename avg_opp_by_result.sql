use chess

select sum(opponent_elo_after) / count(*),count(*),result
from blitz_games
group by 3 desc;

quit
