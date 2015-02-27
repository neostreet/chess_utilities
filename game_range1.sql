use chess

select count(*),result
from blitz_games
where blitz_game_date >= '2015-01-16' and blitz_game_date <= '2015-02-18'
group by 2
order by 1 desc,2 desc;

select count(*)
from blitz_games
where blitz_game_date >= '2015-01-16' and blitz_game_date <= '2015-02-18';

quit
