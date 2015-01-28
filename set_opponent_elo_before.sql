use chess

update blitz_games
set opponent_elo_before = 1367
where opponent_name = 'jhillary' and id = 1;

update blitz_games
set opponent_elo_before = 1120
where opponent_name = 'twiliam20' and id = 2;

quit
