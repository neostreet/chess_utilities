use chess

select count(*),result
from blitz_games
group by 2
order by 2 desc;

select count(*)
from blitz_games;

quit
