use chess

select count(*),eco
from rapid_games
group by eco
order by count(*) desc,eco;

quit
