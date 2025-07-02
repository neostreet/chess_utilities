use chess

select game_filename,game_date,color,opponent_name,num_moves,opening
from rapid_games
where result = 'L' and eco = 'C42'
order by id;

quit
