use chess

select opponent_name,count(*)
from blitz_games
group by opponent_name
order by 2 desc,1 desc;

quit
