@echo off

split_pgn %1 %2
call reverse_it %2_games.lst

build_ch_file .ch.lst .ch %2_games.lst.rev
build_ch_file .bin.lst .bin %2_games.lst.rev

call ftxt2ch_dont %2_games.lst.rev
fch2bin %2_games.ch.lst

call fchess_score_plus_minus_it %2_games.lst.rev
call fchess_rating_diff_it %2_games.lst.rev
call addf_int_it %2_games.lst.rev.fchess_rating_diff

call fchess_wins_it %2_games.lst.rev
rem call fchess_wins_terse_it %2_games.lst.rev
call fchess_rating_diff_it %2_games.lst.rev.fchess_wins
call addf_int_it %2_games.lst.rev.fchess_wins.fchess_rating_diff

call fchess_draws_it %2_games.lst.rev
rem call fchess_draws_terse_it %2_games.lst.rev
call fchess_rating_diff_it %2_games.lst.rev.fchess_draws
call addf_int_it %2_games.lst.rev.fchess_draws.fchess_rating_diff

call fchess_losses_it %2_games.lst.rev
rem call fchess_losses_terse_it %2_games.lst.rev
call fchess_rating_diff_it %2_games.lst.rev.fchess_losses
call addf_int_it %2_games.lst.rev.fchess_losses.fchess_rating_diff

rem type %2_games.lst.rev.fchess_score
call lichess_lines9 %2
rem type %2_games.lst.rev.fchess_wins.fchess_rating_diff.addf_int
rem type %2_games.lst.rev.fchess_draws.fchess_rating_diff.addf_int
rem type %2_games.lst.rev.fchess_losses.fchess_rating_diff.addf_int
rem type %2_games.lst.rev.fchess_rating_diff.addf_int
