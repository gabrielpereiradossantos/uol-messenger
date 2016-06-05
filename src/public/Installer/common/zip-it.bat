@echo off
cls

@echo Zipping emoticons files...
del Emoticons.zip
cd Emoticons
winrar a -afzip -r -x*\Thumbs.db -x*\.svn\* Emoticons.zip Emoticons\*.* Emoticons.xml
move Emoticons.zip ..\Emoticons.zip
cd ..

@echo.
@echo Zipping sounds files...
del Sounds.zip
cd Sounds
winrar a -afzip -r -x*\Thumbs.db -x*\.svn\* Sounds.zip Sounds\*.* Sounds.xml
move Sounds.zip ..\Sounds.zip
cd ..

@echo done.