use chess

select result,count(*),blitz_game_date
from blitz_games
group by 1,3
order by 3,1 desc;

quit