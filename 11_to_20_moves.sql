use chess

select result,count(*)
from blitz_games
where num_moves > 10 and num_moves <= 20
group by result
order by result desc;

select count(*)
from blitz_games
where num_moves > 10 and num_moves <= 20;

quit
