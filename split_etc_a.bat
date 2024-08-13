@echo off

split_pgn %1 %2
call reverse_it %2_games.lst
