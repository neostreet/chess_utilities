use chess

select opponent_elo_after - opponent_elo_before,opponent_name,blitz_game_date
from blitz_games
where opponent_elo_before is not null
order by id;

quit
