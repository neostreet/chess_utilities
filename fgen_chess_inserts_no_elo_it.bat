@echo off
fgen_chess_inserts -no_elo %1 %2 %3 > %3.%1.%2.no_elo.fgen_chess_inserts
call mydl fgen_chess_inserts
