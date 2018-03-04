                ----------------------------------
                     PATTERN TEMPLATE LIBRARY
                ----------------------------------
                         directory TTEST

This directory contains the suite of regression tests, which you can
execute in a single run or individually. Each of the *.cpp files is
test for one pattern. For example, COLLECTI.CPP is the test for the
Collection, you can execute it by typing: ONETEST COLLECTI
This compiles the program, runs it, and compares the result with the
correct result stored in directory OUT.
In order to run the entire regression suite, type:   regr
In order to check the validity of the results, type: check

The test programs from this directory are not only useful for testing
your installation, but also as examples of how to use the library.

Note that the tests in this library run with plain templates, without
using the Template Manager. For tests using the Template Manager, look
at directory MTEST.

WARNING: When running the regression test on Sun, the existing template
         database (directory Templates.DB) must be removed, otherwise
         the compiler may attempt to reuse some templates for classes
         that have the same name, which in some cases may generate
         weird results and errors. This removal is automatically
         done by envir/tmptest, which is derived from envir/sun.tmp.
