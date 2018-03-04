                ----------------------------------
                     PATTERN TEMPLATE LIBRARY
                ----------------------------------
                          ( Ver.1.3 )
                  Copyright Code Farms Inc 2005

Everywhere in this library, by 'pattern' we mean one particular implementation
of a design pattern, in other words a 'concrete' or 'structural pattern'. 

                      INTERNET INSTALLATION

The zip file which you downloaded was created under MS Windows environment.
If you are running under Windows, simply unzip it.
If you are running under Linux or UNIX, do this:
    mkdir ptl
    cd ptl
    unzip -aa ptl.zip

                       MANUAL INSTALLATION

If you received the library on a diskette, the diskette contains a full,
unpacked source of the library, documentation, and the regression test
suite. You can use the library directly from the diskette, or copy
the entire directory tree to your hard disk.

                     SETTING YOUR ENVIRONMENT

The library is written in general C++ using templates, and should run
on any platform, as long as you have a reasonably recent C++ compiler.
The batch files are set up so that the regression tests compile and run
either with Microsoft VC++ or g++/Linux.

The ENVIR directory contains
files which automatically set the environment for various compilers.
For example executing /ptl/envir/setsun sets the environment for the Sun
CC compiles.

There is nothing specifically visual on the patterns and data structures 
stored in this library, and it does not really matter whether you do all
this under DOS or under one of the Windows.
For example, in order to run with Microsoft Visual C++,
enter the DOS mode, and do this:

Step 1: Setup the environment:
          CD C:\PTL\ENVIR
          MSFT
Step 2: Recompile the Template Manager:
          CD C:\PTL\MGR
          ALL

If there is no batch file to set the environment for the compiler which you
are using, you will have to modify several files manually: 
                     
Step 1: Modify files ENVIR\COMPILE.BAT, ENVIR\TMPTEST.BAT, ENVIR\MGRTEST.BAT,
        and MGR\ALL.BAT
        so that they correspond to your operating system and compiler.
        Make the resulting executable. For example, under UNIX, type
          chmod ug+x compile TMPTEST MGRTEST
Step 2: Recompile the Template Processor:
          CD C:\PTL\MGR
          ALL

                           DOCUMENTATION

The documentation (full User Guide) is in the HTML format in the DOC
directory. For more details, read PTL\DOC\README.TXT.


                          SUBDIRECTORIES

LIB = directory which contains files with the templates (*.h) for all
      the patterns and data structures. Each of these files contains
      the equivalent of both the *.h and *.cpp files.

MGR = directory which contains source and executable of 'mgr', the Template
      Manager which does not have to be used, but makes the use of templates
      more user friendly. This directory also contains some utility programs
      such as a custom form of the 'diff' program.

TTEST = directory with tests for individual patterns, using templates only.

MTEST = directory with tests for individual patterns, using the Template
        Manager, mgr.

DOC = directory containing the documentation in the HTML format, plus 
      design notes.

OUT = directory with pre-calculated results for all tests. The results 
      are the same whether using the Template Manager or not.

Files CLEAN.BAT and CLEANIT remove executables and temporary files,
and minimize the size of the library when a copy or transfer is required.

                        FREE DEMO VERSION

The free demo version has the same directory structure, full documentation
(HTML version of the User Guide), and all the test programs normally 
supplied with the production version but, in the LIB directory,
the FSM class is missing. This class is the most valuable
part of this library - it contains a high-performance, dynamically
reconfigurable Finite State Machine, not available from any other library.

Also, in the free demo version, the Template Manager may be missing.

                        MULTIPLE FILES

When you have a separate *.h and *.cpp files for each class,
the 'static' definition of the data organization causes no problem,
because the library classes are only so called interface classes - they
do not have any members (i.e. do not keep any internal values).
