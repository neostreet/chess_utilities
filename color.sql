use chess

select color,count(*)
from blitz_games
group by color
order by color;

quit
