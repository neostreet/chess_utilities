use chess

select count(*),result,blitz_game_date
from blitz_games
where first_two_moves = '1.d4 d5 2.c4 dxc4'
group by 3,2
order by 3,2 desc,1 desc;

quit
