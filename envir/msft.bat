copy msft.cmp compile.bat
copy dos.tmp tmptest.bat
copy dos.mgr mgrtest.bat
copy empty.env ..\lib\environ.h
cd ..\mgr
command/c all
