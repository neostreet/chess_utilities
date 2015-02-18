use chess

select result,count(*)
from blitz_games
where num_half_moves > 80
group by result
order by result desc;

select count(*)
from blitz_games
where num_half_moves > 80;

quit
