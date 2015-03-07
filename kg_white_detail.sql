use chess

select result,opponent_name,blitz_game_date
from blitz_games
where first_two_moves like '1.e4 e5 2.f4%' and
color = 'W'
order by id desc;

quit
