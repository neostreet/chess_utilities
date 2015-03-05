use chess

select sum(num_moves),blitz_game_date
from blitz_games
group by 2
order by 2;

quit
