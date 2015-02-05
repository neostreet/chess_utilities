use chess

select count(*),blitz_game_date,result_detail
from blitz_games
where result = 'L'
group by 2,3
order by 2,1 desc,3;

quit
