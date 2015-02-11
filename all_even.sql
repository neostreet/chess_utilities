use chess

select opponent_name,opponent_elo_before,blitz_game_date
from blitz_games
where opponent_elo_before = opponent_elo_after and
opponent_elo_before = my_elo_before and
my_elo_before = my_elo_after
order by id;

quit
