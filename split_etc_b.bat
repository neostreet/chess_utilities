@echo off

dir /b /on %1_game*.txt > %1_games.txt.lst
unfixtxt %1_games.txt.lst

build_ch_file .ch.lst .ch %1_games.txt.lst
build_ch_file .bin.lst .bin %1_games.txt.lst

call ftxt2ch_dont %2 %1_games.txt.lst
fch2bin %1_games.ch.lst

call fchess_score_plus_minus_it %1_games.txt.lst
call fchess_rating_diff_it %1_games.txt.lst
call addf_int_it %1_games.txt.lst.fchess_rating_diff

call fchess_wins_it %1_games.txt.lst
rem call fchess_wins_terse_it %1_games.txt.lst
call fchess_rating_diff_it %1_games.txt.lst.fchess_wins
call addf_int_it %1_games.txt.lst.fchess_wins.fchess_rating_diff

call fchess_draws_it %1_games.txt.lst
rem call fchess_draws_terse_it %1_games.txt.lst
call fchess_rating_diff_it %1_games.txt.lst.fchess_draws
call addf_int_it %1_games.txt.lst.fchess_draws.fchess_rating_diff

call fchess_losses_it %1_games.txt.lst
rem call fchess_losses_terse_it %1_games.txt.lst
call fchess_rating_diff_it %1_games.txt.lst.fchess_losses
call addf_int_it %1_games.txt.lst.fchess_losses.fchess_rating_diff

rem type %1_games.txt.lst.fchess_score
call lichess_lines9 %1
rem type %1_games.txt.lst.fchess_wins.fchess_rating_diff.addf_int
rem type %1_games.txt.lst.fchess_draws.fchess_rating_diff.addf_int
rem type %1_games.txt.lst.fchess_losses.fchess_rating_diff.addf_int
rem type %1_games.txt.lst.fchess_rating_diff.addf_int
