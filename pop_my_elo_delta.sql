use chess

update blitz_games
set my_elo_delta = my_elo_after - my_elo_before
where my_elo_before is not null;

quit
