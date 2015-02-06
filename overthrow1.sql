use chess

select opponent_name,opponent_elo_before,opponent_elo_after,
my_elo_before,my_elo_after,blitz_game_date
from blitz_games
where opponent_elo_before is not null and
opponent_elo_before > my_elo_before and
opponent_elo_after < my_elo_after
order by id;

quit
