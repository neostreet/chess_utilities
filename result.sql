use chess

select result,count(*)
from blitz_games
group by result
order by result desc;

select count(*)
from blitz_games;

quit
