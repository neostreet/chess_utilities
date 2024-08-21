@echo off
filter_blitz_games -rename %1 > %1.rename.filter_blitz_games
call mydl filter_blitz_games
