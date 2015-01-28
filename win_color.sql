use chess

select color,count(*)
from blitz_games
where result = 'W'
group by color
order by color;

quit
