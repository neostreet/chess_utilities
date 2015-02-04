use chess

select count(*),blitz_game_date
from blitz_games
where result = 'L' and result_detail = 'checkmate'
group by 2
order by 1 desc,2 desc;

quit
