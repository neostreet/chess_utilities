use chess

select time_control,count(*)
from blitz_games
where result = 'D'
group by 1
order by 2 desc,1;

quit
