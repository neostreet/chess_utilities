use chess

select result_decimal,count(*)
from blitz_games
group by result_decimal
order by result_decimal desc;

select count(*)
from blitz_games;

quit
