use chess

select count(*),eco,result
from rapid_games
group by eco,result
order by count(*) desc,eco,result;

quit
