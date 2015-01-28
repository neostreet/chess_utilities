use chess

select color,count(*)
from blitz_games
where result = 'L'
group by color
order by color;

quit
