use chess

select game_filename,game_date
from rapid_games
where eco = 'B52'
order by id;

quit
