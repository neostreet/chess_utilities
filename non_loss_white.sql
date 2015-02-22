use chess

select result != 'L'
from blitz_games
where color = 'W'
order by id;

quit
