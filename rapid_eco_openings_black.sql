use chess

select count(*),eco,opening
from rapid_games
where color = 'B'
group by eco,opening
order by count(*) desc,eco,opening;

quit
