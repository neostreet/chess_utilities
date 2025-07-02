use chess

select count(*),eco,color,result
from rapid_games
group by eco,color,result
order by count(*) desc,eco,color,result;

quit
