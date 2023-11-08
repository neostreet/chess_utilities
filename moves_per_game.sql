use chess

select sum(num_moves) / count(*),sum(num_moves),count(*)
from rapid_games;

quit
