@echo off
call tomysql < \aidan\chess.com\blitz_games_table.sql
call tomysql < blitz_inserts.sql
call tomysql < set_opponent_elo_before.sql
call tomysql < pop_opponent_elo_delta.sql
call tomysql < pop_my_elo_before.sql
call tomysql < pop_my_elo_delta.sql
mysql_grab result
mysql_grab my_elo_after
mysql_grab my_elo_delta_by_date
mysql_grab score_by_date
