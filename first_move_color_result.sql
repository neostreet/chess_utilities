use chess

select count(*),first_move,color,result
from blitz_games
group by 2,3,4
order by 1 desc,2,3 desc,4 desc;

quit
