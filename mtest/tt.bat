..\mgr\mgr composit.cpp
cl -EHsc -I..\lib composit.cpp
composit > composit.res
..\mgr\diff composit.res ..\out\composit.out
