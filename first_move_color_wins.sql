use chess

select count(*),first_move,color
from blitz_games
where result = 'W'
group by 2,3
order by 1 desc,2,3 desc;

quit
