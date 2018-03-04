                ----------------------------------
                     PATTERN TEMPLATE LIBRARY
                ----------------------------------
                         directory envir

This directory sets up the environment (platform and compiler), and
recompiles the Template Manager.

Execute  borland.bat  - in order to run with Borland compiler Ver.4.52+
Execute  msft.bat     - in order to run with Visual C++ Ver.4.1+
Execute  setgnu       - in order to run with GNU++ under UNIX
Execute  setsun       - in order to run with Sun C++
Execute  watcom.bat   - in order to run with Watcom C++ Ver.11.0

Warning: GNU Ver.2.7.2 and earlier have limited handling of templates,
         and cannot be used with this library.

FOR OTHER COMPILERS:

Please, modify the following files manually:
     compile.bat,  tmptest.bat,  mgrtest.bat 

then do this:
     cd ..\mgr
     all

EXPLANATION:
    compile.bat - compiles a program
    tmptest.bat - runs one test with pure templates
    mgrtest.bat - runs one test with the Template Manager

----------------------------------------------------------
WATCOM COMPILER:
Even though the original 1996 version of this library ran under Watcom
compiler Ver.10.6, the present version does not. File WATCOM.BAT in this
directory will execute correctly, but the test programs produce numerous
compiler errors under this compiler. The port to the most recent Watcom
compiler is high on our list.
