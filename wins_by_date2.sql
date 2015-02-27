use chess

select count(*),blitz_game_date from
(select blitz_game_date
from blitz_games
where result = 'W') a
group by blitz_game_date
order by 2 desc;

quit
