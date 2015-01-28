use chess

select color,count(*)
from blitz_games
where result = 'D'
group by color
order by color;

quit
