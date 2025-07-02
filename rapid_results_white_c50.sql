use chess

select count(*),result
from rapid_games
where color = 'W' and eco = 'C50'
group by result
order by count(*) desc,result;

select count(*)
from rapid_games
where color = 'W' and eco = 'C50';

quit
