use chess

update blitz_games
set my_elo_delta = my_elo_after - 1200
where id = 1;

update blitz_games curr
inner join blitz_games prev on curr.id = prev.id + 1
set curr.my_elo_delta = curr.my_elo_after - prev.my_elo_after;

quit
