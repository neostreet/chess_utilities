use chess

select count(*),result
from blitz_games
where color = 'W' and
first_two_moves = '1.d4 d5 2.c4 dxc4'
group by 2
order by 1 desc,2;

quit
