use chess

select result,opponent_name,blitz_game_date
from blitz_games
where first_two_moves = '1.d4 d5 2.c4 dxc4' and
color = 'W'
order by id desc;

quit
