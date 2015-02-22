use chess

select count(*),result
from blitz_games
group by 2
order by 1 desc,2 desc;

select count(*)
from blitz_games;

quit
