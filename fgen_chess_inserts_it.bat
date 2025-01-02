@echo off
fgen_chess_inserts %1 %2 %3 > %3.%1.%2.fgen_chess_inserts
call mydl fgen_chess_inserts
