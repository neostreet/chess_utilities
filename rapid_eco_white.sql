use chess

select count(*),eco
from rapid_games
where color = 'W'
group by eco
order by count(*) desc,eco;

quit
