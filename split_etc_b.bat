@echo off

copy txt.ls0.filter_%1_games %1_games.txt.lst

build_ch_file .ch.lst .ch %1_games.txt.lst
build_ch_file .bin.lst .bin %1_games.txt.lst

call ftxt2ch_dont %2 %1_games.txt.lst
fch2bin %1_games.ch.lst

call tabulate_games_binary_format_it rapid_games.bin.lst
type rapid_games.bin.lst.binary_format.tabulate_games
