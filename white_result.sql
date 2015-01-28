use chess

select result,count(*)
from blitz_games
where color = 'W'
group by result
order by result;

quit
