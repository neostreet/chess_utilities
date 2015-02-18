use chess

select count(*),color,result
from blitz_games
where first_move = '1.d4 Nf6'
group by 2,3
order by 1 desc,2 desc,3 desc;

quit
