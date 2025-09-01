use chess

select count(*),result_detail
from rapid_games
group by 2
order by 1 desc,2;

quit
