use chess

update blitz_games
set result_decimal = 1.0 where result = 'W';

update blitz_games
set result_decimal = 0.0 where result = 'L';

update blitz_games
set result_decimal = 0.5 where result = 'D';

quit
