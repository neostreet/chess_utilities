@echo off
fgen_blitz_inserts %1 %2 %3 > %3.%1.%2.fgen_blitz_inserts
call mydl fgen_blitz_inserts
