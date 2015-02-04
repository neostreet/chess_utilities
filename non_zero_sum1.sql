use chess

select opponent_elo_delta,my_elo_delta,
opponent_elo_delta + my_elo_delta,my_elo_after,result,blitz_game_date
from blitz_games
where opponent_elo_delta is not null and
opponent_elo_delta + my_elo_delta != 0
order by id;

quit
