IF EXIST *.hlt del /S *.hlt
IF EXIST *.log del /S *.log

SET root=cmake-build-debug
.\halite.exe -d "240 160" "..\%root%\MyBot.exe" "..\testbots\halite_orion.exe" "..\testbots\halite_orion.exe" "..\testbots\halite_orion.exe" -s %RANDOM%