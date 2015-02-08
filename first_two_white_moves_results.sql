use chess

select count(*),first_two_moves,result
from blitz_games
where color = 'W'
group by 2,3
order by 1 desc,2,3;

quit
