use chess

select prev.id,prev.game_date prev_game_date,(prev.my_elo_before + prev.my_elo_delta) prev_after,
curr.id,curr.game_date curr_game_date,(curr.my_elo_before) curr_before
from rapid_games curr inner join rapid_games prev on curr.id = prev.id + 1
where curr.my_elo_before != prev.my_elo_before + prev.my_elo_delta;

quit
