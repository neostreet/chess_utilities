use chess

select count(*),first_move,result
from blitz_games
group by 2,3
order by 1 desc,2,3 desc;

quit
