use chess

select game_filename,game_date,opponent_name,eco,opening
from rapid_games
where color = 'W' and result = 'L' and num_moves = 7
order by id;

quit
