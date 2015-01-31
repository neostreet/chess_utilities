use chess

select my_elo_after,count(*)
from blitz_games
group by 1
order by 2 desc,1 desc;

quit
