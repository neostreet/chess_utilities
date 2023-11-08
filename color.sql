use chess

select color,count(*)
from rapid_games
group by color
order by color desc;

quit
