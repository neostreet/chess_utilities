use chess

select opponent_elo_delta,my_elo_delta,blitz_game_date
from blitz_games
where opponent_elo_delta is not null and my_elo_delta is not null
order by id;

quit
