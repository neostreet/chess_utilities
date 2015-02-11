use chess

select opponent_name,result,opponent_elo_before,opponent_elo_after,
my_elo_before,my_elo_after,blitz_game_date
from blitz_games
where opponent_elo_before = my_elo_before
order by id;

quit
