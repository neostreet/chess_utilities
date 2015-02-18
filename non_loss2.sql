use chess

select result != 'L',blitz_game_date
from blitz_games
order by id;

quit
