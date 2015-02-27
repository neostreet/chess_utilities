use chess

select sum(my_elo_delta)
from blitz_games
where blitz_game_date >= '2015-01-16' and blitz_game_date <= '2015-02-18';

quit
