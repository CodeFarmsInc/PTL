// ***********************************************************************
//                       Code Farms Inc.
//          7214 Jock Trail, Richmond, Ontario, Canada, K0A 2Z0
// 
//                     Copyright (C) 1996
//        This software is subject to copyright protection under
//        the laws of Canada, United States, and other countries.
// ************************************************************************

#ifndef PTRARRAY_USED
#include <array.h>
#include <arraycon.h>

// ----------------------------------------------------------------
// Dynamic array of pointers accessed by index. This class is similar 
// to Array<>, but is specifically tuned to handling pointers. The main
//  difference is that in addition to accessing the array through array()[],
// PtrArray provides two more access functions, set() and get().
// These functions not only pass a pointer in and out, but also keep a count
// (on fly) of array entries that are not NULL. Any time you use array()[],
// this count becomes invalid (the class cannot know what you will do with
// the array item once you get access to it). Next time you call count(),
// the count is automatically re-calculated by a pass over the used part
// of the array.
// This is all just a matter of performance: If you don't need the count,
// use array()[], set(), and get() arbitrarily. If you need the count
// and you are concerned about performance, avoid array()[], and
// access the array only through set() and get().
// If entry at index i is not NULL, and you call set(p,i,NULL), the count
// will be decremented. 
// The constructor automatically initializes all entries as NULL.
// Note the difference in remove() which here returns the value
// of the pointer which was removed.
// Function inital() has only one parameter (starting size). The array
// is always initialized to NULL.
// The index is addressed by an (unsigned int) index; using a (short)
// or (char) as the index automatically converts it to (unsigned int).
// ??? iterator is needed!!


// ------------------ ptrarray.g --------------------
// pattern PtrArray<H,T,i>
// where H is the class which holds the array
//       T is the class of the objects to which the pointers point,
//       i is the integer index, usually 0
// --------------------------------------------------
// Conditions: NONE
//   (1) H must inherit PtrArrayHolder<H,T,i>
//   (2) T is the class of the objects stored in the array
// --------------------------------------------------
// Example:
// class Department : public PtrArrayHolder<Department,Employee,0> }
//    ...
// };
// class Employee {
//    ...
// };
// PtrArray<Department,Employee,0> myPtrArray;
// --------------------------------------------------
// Author: Jiri Soukup, Feb.28/95, completely reworked Feb.4/97
// --------------------------------------------------

#define oType PtrArray<H,T,i>
#define hType PtrArrayHolder<H,T,i>
#define cType ArrayContainer<T*>

template<class H,class T,int i> class PtrArray;

#ifdef PTL_CANNOT_INITIALIZE
template<class H,class T,int i> class PtrArrayHolder : public cType {
#else

template<class H,class T,int i=0> class PtrArrayHolder : public cType {
#endif
friend class oType;
    static int sortCmp(const void *v1,const void *v2){int ii=0;
        long k=(long)v1 - (long)v2; if(k<0)ii= -1; else if(k>0)ii=1; return ii;}
protected:
    unsigned int cnt; // counts entries that are not NULL
    int onFly;  // 1 when calculated on fly, 0 when out of synch
    static unsigned int startingSz; // not static in ArrayContainer
    T* remove(const unsigned int k){T* t=NULL;
        if(k<num)t=array[k]; if(t)cnt--; cType::remove(k); return t;}
    T* fastRemove(const unsigned int k){T* t=NULL;
        if(k<num)t=array[k]; if(t)cnt--; cType::fastRemove(k); return t;}
    void push(T* t){if(t)cnt++; cType::push(&t);}
    T* pop(){T* t= *(cType::pop()); if(t)cnt--; return t;}
    int cut(const unsigned int k){onFly=0; return cType::cut(k);}
    unsigned int count();
    void insert(const unsigned int k,T *t){if(t)cnt++; cType::insert(k,&t);}
    void set(const unsigned int k,T *t);
    T *get(const unsigned int k){T *t=NULL; if(k<sz)t=array[k]; return t;}
    void clean();
public:
    PtrArrayHolder():cType(startingSz,1){cnt=0; onFly=1;};
};

#ifdef PTL_CANNOT_INITIALIZE
template<class H,class T,int i> class PtrArray {
#else

template<class H,class T,int i=0> class PtrArray {
#endif
protected:
typedef int (*cmpType)(const void *, const void *);
public:
    void initial(unsigned int const startSz){ hType::startingSz=startSz;}
    unsigned int size(H* h) const{return ((hType*)h)->size();}
    unsigned int used(H* h) const{return ((hType*)h)->used();}
    hType& array(H *h){return *((hType*)h);} 
               // main access to the array - syntax: id.array(h)[k]
    void insert(H *h,const unsigned int k,T *t){((hType*)h)->insert(k,t);}
    void push(H *h,T* t){((hType*)h)->push(t);}
    T* pop(H *h){return ((hType*)h)->pop();}
    int reduce(H *h){return ((hType*)h)->reduce();}
                                       // reduce the array to its used size
    int cut(H *h,const unsigned int k){return ((hType*)h)->cut(k);}
                                       // cut the array to accomodate index k
    int grow(H *h,const unsigned int k){return ((hType*)h)->grow(k);}
                                       // grow the array to accomodate index k
    void sort(H *h,cmpType cmp){((hType*)h)->sort(cmp);} // sort with qsort
    unsigned int count(H *h){return ((hType*)h)->count();} 
                                          // count of entries that are not NULL
    T *get(H *h,const unsigned int k){return ((hType*)h)->get(k);}
    void set(H *h,const unsigned int k,T *t){((hType*)h)->set(k,t);}
    T* remove(H *h,const unsigned int k){return ((hType*)h)->remove(k);}
    T* fastRemove(H *h,const unsigned int k){return ((hType*)h)->fastRemove(k);}
    void clean(H *h){((hType*)h)->clean();}
};

template<class H,class T,int i> unsigned int hType::startingSz=8;

template<class H,class T,int i> void hType::set(const unsigned int k,T *t){
    T* p=NULL; if(k<num)p=array[k];
    if(k>=sz)sizeChange(k,0);
    if(k<sz){ // always true unless sizeChange() fails
        if(k>=num)num=k+1;
        if(!p && t)cnt++; else if(p && !t)cnt--;
        array[k]=t;
    }
}
// --------------------------------------------------------------
// If only set() and get() are used, the count is updated on fly,
// and we can return it. Any time operator[] is called, this mechanism
// breaks down, and the count must be calculated by a pass over the 
// entire used section.
// --------------------------------------------------------------
template<class H,class T,int i> unsigned int hType::count() {
    unsigned int k;
    if(onFly)return cnt;
    for(k=cnt=0; k<num; k++) if(array[k])cnt++;
    onFly=1;
    return cnt;
}

// --------------------------------------------------------------
// Clean up the objects to which pointer from this array lead.
// If any object is pointed to from more than one place, this could
// cause a disaster. The fastest way to eliminate duplications is
// to sort the array first, and then destroy only the first object 
// from each group of equal pointers.
// --------------------------------------------------------------
template<class H,class T,int i> void hType::clean() {
    unsigned int k;
    T *t;

    sort(hType::sortCmp);
    for(k=0; k<sz; k++){
        t=array[k];
        if(t){
            if(k>0 && t!=array[k-1])delete t;
            array[k]=NULL;
        }
    }
    sz=num=cnt=0;
}

// cancel all temporary renaming
#undef oType
#undef hType
#undef cType

#ifdef TEMPLATE_MANAGER
// --------------------------------------------------
// template manager is not used except for moving the file into pattern.h
#define PtrArrayInherit1(id,holder,elem,i) public PtrArrayHolder<holder,elem,i>
#define PtrArrayInherit2(id,holder,elem,i)
#define PtrArrayMember1(id,holder,elem,i)
#define PtrArrayMember2(id,holder,elem,i)
// --------------------------------------------------
#endif // TEMPLATE_MANAGER

#endif // PTRARRAY_USED
#define PTRARRAY_USED
