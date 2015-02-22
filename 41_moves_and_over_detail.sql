use chess

select result,num_moves,opponent_name,blitz_game_date
from blitz_games
where num_moves > 40
order by id desc;

quit
