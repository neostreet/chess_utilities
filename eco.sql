use chess

select eco,count(*)
from blitz_games
group by 1
order by 2 desc,1;

quit
