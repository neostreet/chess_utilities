@echo off
fchess_score -plus_minus neostreet %1 > %1.plus_minus.fchess_score
call mydl fchess_score
