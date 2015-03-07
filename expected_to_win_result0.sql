use chess

select count(*)
from blitz_games
where my_elo_after > opponent_elo_after
group by result
order by result desc;

quit
