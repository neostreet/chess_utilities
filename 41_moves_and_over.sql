use chess

select result,count(*)
from blitz_games
where num_moves > 40
group by result
order by result desc;

select count(*)
from blitz_games
where num_moves > 40;

quit
