@echo off
split_pgn %1
call reverse_it games.lst
call frapid_it games.lst.rev
ftxt2ch games.lst.rev.frapid

call fchess_score_plus_minus_it games.lst.rev.frapid
call fchess_rating_diff_it games.lst.rev.frapid
call addf_int_it games.lst.rev.frapid.fchess_rating_diff

call fchess_wins_it games.lst.rev.frapid
rem call fchess_wins_terse_it games.lst.rev.frapid
call fchess_rating_diff_it games.lst.rev.frapid.fchess_wins
call addf_int_it games.lst.rev.frapid.fchess_wins.fchess_rating_diff

call fchess_draws_it games.lst.rev.frapid
rem call fchess_draws_terse_it games.lst.rev.frapid
call fchess_rating_diff_it games.lst.rev.frapid.fchess_draws
call addf_int_it games.lst.rev.frapid.fchess_draws.fchess_rating_diff

call fchess_losses_it games.lst.rev.frapid
rem call fchess_losses_terse_it games.lst.rev.frapid
call fchess_rating_diff_it games.lst.rev.frapid.fchess_losses
call addf_int_it games.lst.rev.frapid.fchess_losses.fchess_rating_diff

rem type games.lst.rev.frapid.fchess_score
call lichess_lines9
rem type games.lst.rev.frapid.fchess_wins.fchess_rating_diff.addf_int
rem type games.lst.rev.frapid.fchess_draws.fchess_rating_diff.addf_int
rem type games.lst.rev.frapid.fchess_losses.fchess_rating_diff.addf_int
rem type games.lst.rev.frapid.fchess_rating_diff.addf_int
