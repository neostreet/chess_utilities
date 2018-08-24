use chess

update blitz_games
set opponent_elo_after = opponent_elo_before + opponent_elo_delta,
my_elo_after = my_elo_before + my_elo_delta;

quit
