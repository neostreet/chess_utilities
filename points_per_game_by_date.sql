use chess

select sum(result_decimal) / count(*),blitz_game_date
from blitz_games
group by blitz_game_date
order by blitz_game_date;

quit
