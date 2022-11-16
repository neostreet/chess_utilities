@echo off
pushd c:\users\lloyd\cygwin_tools
call copy1
popd
fchess_wins neostreet %1 > %1.fchess_wins
call mydl fchess_wins
