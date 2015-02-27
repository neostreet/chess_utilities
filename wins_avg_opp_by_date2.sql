use chess

select sum(opponent_elo_after) / count(*),count(*),blitz_game_date
from blitz_games
where result = 'W'
group by blitz_game_date
order by 1 desc,3 desc;

quit
