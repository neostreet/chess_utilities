use chess

select num_moves,color,blitz_game_date,id
from blitz_games
where result = 'W' and result_detail = 'checkmate'
order by id;

quit
