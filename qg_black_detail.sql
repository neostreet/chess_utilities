use chess

select result,opponent_name,blitz_game_date
from blitz_games
where first_two_moves like '1.d4 d5 2.c4%' and
color = 'B'
order by id desc;

quit
