use chess

select curr.my_elo_after - prev.my_elo_after,curr.my_elo_after,curr.result,curr.blitz_game_date
from blitz_games prev,blitz_games curr
where prev.id = curr.id - 1
order by curr.id;

quit
