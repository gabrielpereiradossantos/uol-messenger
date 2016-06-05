@echo off
cls

@echo Zipping Skin files...
del Default.zip
winrar a -afzip -r -x*\Thumbs.db -x*\.svn\* Default.zip Skin\*.* Skin.xml

@echo done.