use chess

select count(*),opening
from rapid_games
where eco in ('D30','D53')
group by opening
order by count(*) desc,opening;

quit
