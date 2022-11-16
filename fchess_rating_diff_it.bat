@echo off
fchess_rating_diff neostreet %1 > %1.fchess_rating_diff
call mydl fchess_rating_diff
