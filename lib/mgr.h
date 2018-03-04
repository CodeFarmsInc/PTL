// ***********************************************************************
//                       Code Farms Inc.
//          7214 Jock Trail, Richmond, Ontario, Canada, K0A 2Z0
// 
//                     Copyright (C) 1996
//        This software is subject to copyright protection under
//        the laws of Canada, United States, and other countries.
// ************************************************************************
#ifndef MGR_USED
#define MGR_USED
#include <environ.h>
#include <stdlib.h>

#define pattern
#define Pattern(A) pattern_##A
#define Iterator(A) pattern_iterator_##A
#define ITERATE(it,par,p) for((p)=(it).begin(par); p; (p)= ++(it))
#define RETRACE(it,par,p) for((p)=(it).end(par); p; (p)= --(it))

#ifdef PTL_CANNOT_COMMENT_OUT
#define PTL_COMMENT
#else
#define PTL_COMMENT /##/
#endif

// ---------- ERROR HANDLING FOR THE ENTIRE LIBRARY -----------------
#include <iostream>
using namespace std;

class PTLerrorHandler{
public:
    char *msg;
    void *ptr;
    PTLerrorHandler(char *m,void *p){msg=m; ptr=p;}
    static void error(char *msg,void *ptr){
        cout << "ERROR: " << msg << " " << (unsigned long)ptr << "\n";
        cout.flush();
        throw PTLerrorHandler(msg,ptr);
    }
    static void warning(char *msg,void *ptr){
        cout << "WARNING: " << msg << " " << (unsigned long)ptr << "\n";
        cout.flush();
    }
};

#define PTLerror PTLerrorHandler::error
#define PTLwarning PTLerrorHandler::warning


#endif // MGR_USED
