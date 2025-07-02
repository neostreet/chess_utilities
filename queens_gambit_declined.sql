use chess

select count(*),eco
from rapid_games
where opening = "Queen's Gambit Declined"
group by eco
order by count(*) desc,eco;

quit
