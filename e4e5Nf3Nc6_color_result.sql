use chess

select count(*),color,result
from blitz_games
where first_two_moves = '1.e4 e5 2.Nf3 Nc6'
group by 2,3
order by 1 desc,2,3;

quit
