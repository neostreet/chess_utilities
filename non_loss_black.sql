use chess

select result != 'L'
from blitz_games
where color = 'B'
order by id;

quit
