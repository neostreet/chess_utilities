use chess

select blitz_game_date,count(*)
from blitz_games
group by blitz_game_date
order by id;

quit
