use chess

select count(*),result
from rapid_games
group by 2
order by count(*) desc,2 desc;

select count(*)
from rapid_games;

quit
