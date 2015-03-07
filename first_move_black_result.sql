use chess

select count(*),first_move,result
from blitz_games
where color = 'B' and first_move is not null
group by 2,3
order by 1 desc,2,3 desc;

select count(*)
from blitz_games
where color = 'B' and first_move is not null;

quit
