:: syntax: build.bat <filename with no extension> <file type - exe/com>

@echo off

set @name=%1

set @asm=%@name%.asm
set @obj=%@name%.obj
set @exe=%@name%.exe
set @com=%@name%.com

@echo on

masm %@asm%;
link %@obj%;

@echo off

set @type=%2
if %@type%==com exe2bin %@exe% %@com%
if %@type%==com del %@exe%

del %@obj%

set @name=
set @asm=
set @obj=
set @exe=
set @com=
set @type=

@echo on
