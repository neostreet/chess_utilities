use chess

select count(*),result,result_detail
from rapid_games
group by 2,3
order by 1 desc,2,3;

quit
