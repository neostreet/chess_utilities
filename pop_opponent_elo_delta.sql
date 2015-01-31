use chess

update blitz_games
set opponent_elo_delta = opponent_elo_after - opponent_elo_before
where opponent_elo_before is not null;

quit
