use chess

select count(*),result
from blitz_games
where color = 'B' and
first_two_moves = '1.e4 e5 2.Nf3 Nc6'
group by 2
order by 1 desc,2;

quit
