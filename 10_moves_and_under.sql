use chess

select result,count(*)
from blitz_games
where num_half_moves <= 20
group by result
order by result desc;

select count(*)
from blitz_games
where num_half_moves <= 20;

quit
