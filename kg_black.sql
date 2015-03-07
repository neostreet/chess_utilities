use chess

select count(*),result
from blitz_games
where first_two_moves like '1.e4 e5 2.f4%' and
color = 'B'
group by 2
order by 1 desc,2 desc;

select count(*)
from blitz_games
where first_two_moves like '1.e4 e5 2.f4%' and
color = 'B';

quit
