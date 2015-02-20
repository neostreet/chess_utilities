use chess

select count(*),left(first_move,4)
from blitz_games
where color = 'W' and first_move is not null
group by 2
order by 1 desc,2;

quit
