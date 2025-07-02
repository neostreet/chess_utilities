use chess

select count(*),eco
from rapid_games
where color = 'B'
group by eco
order by count(*) desc,eco;

quit
