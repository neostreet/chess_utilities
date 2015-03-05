use chess

select result = 'W' and result_detail = 'checkmate'
from blitz_games
order by id;

quit
