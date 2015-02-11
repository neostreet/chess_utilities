use chess

select id,opponent_name
from blitz_games
where opponent_elo_before is null
order by id;

quit
