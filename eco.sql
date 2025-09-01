use chess

select count(*),eco
from rapid_games
group by 2
order by count(*) desc,2 desc;

select count(*)
from rapid_games;

quit
