@echo off
call tomysql < \aidan\chess.com\blitz_games_table.sql
call tomysql < blitz_inserts.sql
call tomysql < set_opponent_elo_before.sql
call tomysql < pop_opponent_elo_delta.sql
call tomysql < pop_my_elo_before.sql
call tomysql < pop_my_elo_delta.sql
mysql_grab result
mysql_grab result_detail
mysql_grab wins_by_type
mysql_grab losses_by_type
mysql_grab white_result
mysql_grab black_result
mysql_grab my_elo_after
mysql_grab my_elo_delta_by_date
mysql_grab score_by_date
mysql_grab first_two_moves
mysql_grab first_two_white_moves
mysql_grab first_two_black_moves
mysql_grab first_two_white_moves_results
mysql_grab first_two_black_moves_results
mysql_grab avg_opp
