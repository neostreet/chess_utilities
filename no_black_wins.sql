use chess

select distinct blitz_game_date
from blitz_games
where blitz_game_date not in (
  select distinct blitz_game_date
  from blitz_games
  where result = 'W' and color = 'B')
order by blitz_game_date desc;

quit
