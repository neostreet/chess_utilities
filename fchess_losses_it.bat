@echo off
fchess_losses neostreet %1 > %1.fchess_losses
call mydl fchess_losses
