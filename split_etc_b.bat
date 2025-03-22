@echo off

dir /b /on %1_game*.txt > %1_games.txt.lst
unfixtxt %1_games.txt.lst

build_ch_file .ch.lst .ch %1_games.txt.lst
build_ch_file .bin.lst .bin %1_games.txt.lst

call ftxt2ch_dont %2 %1_games.txt.lst
fch2bin %1_games.ch.lst
