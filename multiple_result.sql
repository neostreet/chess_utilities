use chess

select result,count(*)
from blitz_games
where opponent_name in
(select opponent_name from blitz_games
group by opponent_name
having count(*) > 1)
group by result
order by result desc;

select count(*)
from blitz_games
where opponent_name in
(select opponent_name from blitz_games
group by opponent_name
having count(*) > 1);

quit
