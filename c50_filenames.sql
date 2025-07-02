use chess

select game_filename
from rapid_games
where eco = 'C50'
order by id;

quit
