                ----------------------------------
                     PATTERN TEMPLATE LIBRARY
                ----------------------------------
                         directory MTEST

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

Note that the tests in this library use Template Manager. For tests
running with pure templates (without the Template Manager), look at
directory TTEST.
