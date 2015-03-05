use chess

select count(*),result
from blitz_games
where my_elo_after = opponent_elo_after
group by 2
order by 2 desc;

select count(*)
from blitz_games
where my_elo_after = opponent_elo_after;

quit
