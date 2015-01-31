use chess

select count(*) from blitz_games curr
inner join blitz_games prev on curr.id = prev.id + 1
where curr.my_elo_before != prev.my_elo_after;

quit
