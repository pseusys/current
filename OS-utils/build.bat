:: syntax: build.bat <filename with no extension> <file type - exe/com>

@echo off

set @name=%1

set @asm=%@name%.asm
set @obj=%@name%.obj
set @exe=%@name%.exe
set @com=%@name%.com

masm %@asm%; > nul
link %@obj%; > log.txt

set @type=%2
if %@type%==com exe2bin %@exe% %@com%

@echo on