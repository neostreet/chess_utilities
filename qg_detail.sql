use chess

select result,color,opponent_name,blitz_game_date
from blitz_games
where first_two_moves like '1.d4 d5 2.c4%'
order by id desc;

quit
