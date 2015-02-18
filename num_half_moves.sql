use chess

select num_half_moves,count(*)
from blitz_games
group by num_half_moves
order by num_half_moves desc;

quit
