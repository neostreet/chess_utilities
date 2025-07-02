use chess

select count(*),eco,opening
from rapid_games
where eco in ('D30','D53')
group by eco,opening
order by count(*) desc,eco,opening;

quit
