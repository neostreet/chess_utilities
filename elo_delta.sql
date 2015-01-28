use chess

select curr.opponent_elo_after - curr.opponent_elo_before,curr.opponent_name,
curr.my_elo_after - prev.my_elo_after,curr.blitz_game_date
from blitz_games prev,blitz_games curr
where prev.id = curr.id - 1 and
curr.opponent_elo_before is not null
order by curr.id;

quit
