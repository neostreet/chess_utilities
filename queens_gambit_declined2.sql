use chess

select count(*),eco,opening
from rapid_games
where opening like "Queen's Gambit Declined%"
group by eco,opening
order by count(*) desc,eco,opening;

quit
