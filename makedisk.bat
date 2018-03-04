command/c clean
mkdir %1:\ptl
copy *.* %1:\ptl
mkdir %1:\ptl\mgr
cd mgr
copy *.* %1:\ptl\mgr
mkdir %1:\ptl\lib
cd ..\lib
copy *.* %1:\ptl\lib
mkdir %1:\ptl\ttest
cd ..\ttest
copy *.* %1:\ptl\ttest
mkdir %1:\ptl\mtest
cd ..\mtest
copy *.* %1:\ptl\mtest
mkdir %1:\ptl\out
cd ..\out
copy *.* %1:\ptl\out
mkdir %1:\ptl\envir
cd ..\envir
copy *.* %1:\ptl\envir
mkdir %1:\ptl\doc
cd ..\doc
copy *.* %1:\ptl\doc
cd ..
