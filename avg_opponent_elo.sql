use chess

select sum(opponent_elo_before) / count(*)
from blitz_games
where opponent_elo_before is not null;

select sum(opponent_elo_before) / count(*)
from blitz_games
where opponent_elo_before is not null and
result = 'W';

select sum(opponent_elo_before) / count(*)
from blitz_games
where opponent_elo_before is not null and
result = 'L';

select sum(opponent_elo_before) / count(*)
from blitz_games
where opponent_elo_before is not null and
result = 'D';

quit
