use chess

select result_detail,count(*)
from blitz_games
where result = 'L'
group by 1
order by 2 desc,1;

quit
