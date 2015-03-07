use chess

select count(*),result
from blitz_games
where first_move like '1.e4%' and color = 'W'
group by 2
order by 1 desc,2 desc;

select count(*)
from blitz_games
where first_move like '1.e4%' and color = 'W';

quit
