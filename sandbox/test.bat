IF EXIST *.hlt del /S *.hlt
IF EXIST *.log del /S *.log

SET root=cmake-build-debug
.\halite.exe -d "240 160" "..\%root%\MyBot.exe" "..\MyBot.exe" -s 1337