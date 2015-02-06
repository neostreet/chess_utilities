use chess

select result,count(*)
from blitz_games
where opponent_elo_before is not null and my_elo_before > opponent_elo_before
group by result
order by result desc;

quit
