rem ------- aggregat --------
cl -EHsc -I..\lib aggregat.cpp
del aggregat.res
aggregat > aggregat.res
..\mgr\diff aggregat.res ..\out\aggregat.out
rem ------- array --------
cl -EHsc -I..\lib array.cpp
del array.res
array > array.res
..\mgr\diff array.res ..\out\array.out
rem ------- collecti --------
cl -EHsc -I..\lib collecti.cpp
del collecti.res
collecti > collecti.res
..\mgr\diff collecti.res ..\out\collecti.out
rem ------- composit --------
cl -EHsc -I..\lib composit.cpp
del composit.res
composit > composit.res
..\mgr\diff composit.res ..\out\composit.out
rem ------- flyweigh --------
cl -EHsc -I..\lib flyweigh.cpp
del flyweigh.res
flyweigh > flyweigh.res
..\mgr\diff flyweigh.res ..\out\flyweigh.out
rem ------- ptrarray --------
cl -EHsc -I..\lib ptrarray.cpp
del ptrarray.res
ptrarray > ptrarray.res
..\mgr\diff ptrarray.res ..\out\ptrarray.out
rem ------- arraycon --------
cl -EHsc -I..\lib arraycon.cpp
del arraycon.res
arraycon > arraycon.res
..\mgr\diff arraycon.res ..\out\arraycon.out
rem ------- fsm --------
cl -EHsc -I..\lib fsm.cpp
del fsm.res
fsm > fsm.res
..\mgr\diff fsm.res ..\out\fsm.out
