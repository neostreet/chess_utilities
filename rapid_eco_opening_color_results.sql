use chess

select count(*),eco,opening,color,result
from rapid_games
group by eco,opening,color,result
order by count(*) desc,eco,opening,color,result;

quit
