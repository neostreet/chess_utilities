use chess

select count(*),first_four_moves
from blitz_games
group by 2
order by 1 desc,2;

quit
