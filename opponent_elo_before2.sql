use chess

select opponent_elo_before,opponent_name,result,blitz_game_date
from blitz_games
where opponent_elo_before is not null
order by 1 desc,id desc;

quit
