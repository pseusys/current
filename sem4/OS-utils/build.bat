:: syntax: build.bat <filename with no extension> <file type - exe/com/ovl>

@echo off

set @name=%1

set @asm=%@name%.asm
set @obj=%@name%.obj
set @exe=%@name%.exe

@echo on

masm %@asm%;
link %@obj%;

@echo off

set @type=%2
set @form=%@name%.%@type%
if not %@type%==exe exe2bin %@exe% %@form%
if not %@type%==exe del %@exe%

del %@obj%

set @name=
set @asm=
set @obj=
set @exe=
set @com=
set @type=

@echo on
