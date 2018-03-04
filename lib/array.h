// ***********************************************************************
//                       Code Farms Inc.
//          7214 Jock Trail, Richmond, Ontario, Canada, K0A 2Z0
// 
//                     Copyright (C) 1996
//        This software is subject to copyright protection under
//        the laws of Canada, United States, and other countries.
// ************************************************************************

#ifndef ARRAY_USED
#include <mgr.h>
#include <stdlib.h>
#include <arraycon.h>
// ---------- array.g --------------------
// Array<> is a dynamic array of general objects addressed by index.
// It is is based on ArrayContainer<>, but instead of being a self-standing
// array, it is a data organization attached to a give application class.
// This may appear unnecessary (why not to use ArrayContainer as a member
// of that class?), but it make its interface and use uniform with
// the intrusive, pointer based data structures and patterns.
//
// When working with this pattern, 3 classes are involved:
//   H - the holder class, which holds the array
//   T - the element class (objects of this class are in the array)
//   i - integer index to handle multiple organizations
//
// Class Array<> can handle not only arrays of objects, but also arrays
// of pointers. For example:   Array<Department,Employee*,0>
// where each Department stores and array of Employee pointers.
// However, this library provides also class PtrArray<> which avoids
// confusion about too many '*' in the declarations, and also provides
// additional functionality (count of pointers that are not NULL, and
// returning the value of the pointer for some operations).
// 
// Class ArrayHolder<H,T,i> provides a 3-parameter template cover for
// the single parameter ArrayContainer<T>. The 3 parameters are needed
// for Array<H,T,i>. ArrayHolder<> is the part which will be attached to
// the application class which will hold the array.
//
// The index is assumed to be 'unsigned int'. Using a 'short' or a 'char'
// as index automatically converts to 'unsigned int'. We will use this 
// feature in some patterns (flyweight).
// --------------------------------------------------
// Conditions that must satisfied when using this pattern: 
//   (1) H is a subclass of ArrayHolder<H,T,i>
//   (2) T is the type of the objects to store in the array
// --------------------------------------------------
// Algorithm:
// The constructor specifies the starting size for the array, and also
// the type of initialization (none, by 0, or by blank).
// When accessing the array with an index which is >= than the current
// size, the array doubles its size until sufficient. For architectures where
// sizeof(int)==2, such as WIN/DOS, the size of the array is limited to 64kB
// Note that for efficiency reasons, pop() coes not clean the released space.
// When accessing the array, you have to specify the holder object and
// the index. For example:
//     class Person;
//     class Account;
//     Array<P,A> accounts;
//     Person *p; Account *a;
//      ...
//     accounts.array(p)[7]=(*a);
//     (*a)=accounts.array(p)[0];
//     if(accounts.array(p)[3] == accounts.array(p)[4]){ ... }
// ??? iterators much needed now
// There is no need for iterators, since all the objects can be accessed
// via an unsigned int index, and the size of the array is available through
// functions used() and size(). In order to traverse the used part of
// the array, do this:
//      for(i=0; i<myArray.used(); i++){ ...=myArray[i];}
// In order to access the presently allocated array, do this:
//      for(i=0; i<myArray.size(); i++){ ...=myArray[i];}
// sort() is based on qsort, and expects the same type of compare function.
// --------------------------------------------------
// When working with an array of pointers, do this:
//   class A;
//   Array<A*> myArray;
// --------------------------------------------------
// Author: Jiri Soukup, Feb.28/95, change to holder object Feb.4/97
// --------------------------------------------------

// the following defines are not true macros, they just locally
// replace long templates by shorter names
#define oType Array<H,T,i>
#define hType ArrayHolder<H,T,i>

template<class H,class T,int i> class Array;

#ifdef PTL_CANNOT_INITIALIZE
template<class H,class T,int i> class ArrayHolder : public ArrayContainer<T>{
#else

template<class H,class T,int i=0> class ArrayHolder : public ArrayContainer<T>{
#endif
friend class oType;
    static unsigned int initialMode; // not static in ArrayContainer
    static unsigned int startingSz; // not static in ArrayContainer
public:
    ArrayHolder():ArrayContainer<T>(startingSz,initialMode){};
};

#ifdef PTL_CANNOT_INITIALIZE
template<class H,class T,int i> class Array {
#else

template<class H,class T,int i=0> class Array {
#endif
protected:
typedef int (*cmpType)(const void *, const void *);
public:
    void initial(unsigned int const startSz,unsigned int const ini){
                         hType::startingSz=startSz; hType::initialMode=ini;}
                                  // 0=no initialization,1=by NULL,2=by blank
    unsigned int size(H* h) const{return(((hType*)h)->sz);}
    unsigned int used(H* h) const{return(((hType*)h)->num);}
    hType& array(H *h){return *((hType*)h);} 
                                  // main access to the array - see explanation
    void remove(H *h,const unsigned int k){((hType*)h)->remove(k);} 
                                            // remove and shrink remaining part
    void fastRemove(H *h,const unsigned int k){((hType*)h)->fastRemove(k);} 
                                            // remove without maintaining order
    void insert(H *h,const unsigned int k,T *t){((hType*)h)->insert(k,t);}
    void push(H *h,T* e){((hType*)h)->push(e);}
    T* pop(H *h){return ((hType*)h)->pop();}
    int reduce(H *h){return ((hType*)h)->reduce();}
                                       // reduce the array to its used size
    int cut(H *h,const unsigned int k){return ((hType*)h)->cut(k);}
                                       // cut the array to accomodate index k
    int grow(H *h,const unsigned int k){return ((hType*)h)->grow(k);}
                                       // grow the array to accomodate index k
    void sort(H *h,cmpType cmp){((hType*)h)->sort(cmp);} // sort using qsort
};

template<class H,class T,int i> unsigned int hType::initialMode=0;
template<class H,class T,int i> unsigned int hType::startingSz=8;

// cancel all temporary renaming
#undef oType
#undef hType

#ifdef TEMPLATE_MANAGER
// --------------------------------------------------
// template manager is not used except for moving the file into pattern.h
#define ArrayInherit1(id,holder,elem,i) public ArrayHolder<holder,elem,i>
#define ArrayInherit2(id,holder,elem,i)
#define ArrayMember1(id,holder,elem,i)
#define ArrayMember2(id,holder,elem,i)
// --------------------------------------------------
#endif // TEMPLATE_MANAGER

#endif // ARRAY_USED
#define ARRAY_USED
