use chess

select count(*),result
from blitz_games
where first_two_moves like '1.d4 d5 2.c4%' and
color = 'W'
group by 2
order by 1 desc,2 desc;

select count(*)
from blitz_games
where first_two_moves like '1.d4 d5 2.c4%' and
color = 'W';

quit
