use chess

select count(*),result,color
from blitz_games
group by 2,3
order by 1 desc,2 desc,3 desc;

select count(*)
from blitz_games;

quit
