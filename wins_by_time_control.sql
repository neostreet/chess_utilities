use chess

select time_control,count(*)
from blitz_games
where result = 'W'
group by 1
order by 2 desc,1;

quit