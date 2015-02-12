use chess

select result,count(*)
from blitz_games
where color = 'B'
group by result
order by result desc;

select count(*)
from blitz_games
where color = 'B';

quit
