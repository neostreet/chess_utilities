use chess

select count(*),eco,opening,result
from rapid_games
where color = 'W'
group by eco,opening,result
order by count(*) desc,eco,opening,result;

quit
