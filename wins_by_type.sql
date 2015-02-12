use chess

select result_detail,count(*)
from blitz_games
where result = 'W'
group by 1
order by 2 desc,1;

select count(*)
from blitz_games
where result = 'W';

quit
