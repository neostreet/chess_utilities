use chess

select count(*),first_two_moves
from blitz_games
where color = 'W'
group by 2
order by 1 desc,2;

quit
