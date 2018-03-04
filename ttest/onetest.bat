command/c ..\envir\compile %1
%1 > %1.res
command/c ..\mgr\diff %1.res ..\out\%1.out
