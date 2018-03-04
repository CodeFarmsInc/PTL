cl -EHsc -I..\lib composit.cpp
del composit.res
composit > composit.res
..\mgr\diff composit.res ..\out\composit.out
