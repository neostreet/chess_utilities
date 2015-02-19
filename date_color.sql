use chess

select count(*),blitz_game_date,color
from blitz_games
group by blitz_game_date,color
order by blitz_game_date,color desc;

quit
