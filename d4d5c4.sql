use chess

select count(*),color,result
from blitz_games
where first_two_moves like '1.d4 d5 2.c4%'
group by 2,3
order by 1 desc,2 desc,3 desc;

quit
