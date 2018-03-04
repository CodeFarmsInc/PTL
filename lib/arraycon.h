// ***********************************************************************
//                       Code Farms Inc.
//          7214 Jock Trail, Richmond, Ontario, Canada, K0A 2Z0
// 
//                     Copyright (C) 1996
//        This software is subject to copyright protection under
//        the laws of Canada, United States, and other countries.
// ************************************************************************

#ifndef ARRAYCON_USED
#include <mgr.h>
#include <stdlib.h>
// ---------- arraycon.g --------------------
// This is the classical representation of the dynamic array.
// It is a self-standing class, nobody must inherit it, and it does not 
// inherit from any other class. This array can store either entire objects
// or just pointers to objects.
// --------------------------------------------------
// Conditions that must be satisfied when using this pattern:  NONE
// --------------------------------------------------
// Algorithm:
// The constructor allows to specify the initial size and whether/how
// to initialize the array.
// When accessing the array with an index which is >= than the current
// size, the array doubles its size until sufficient. For architectures where
// sizeof(int)==2, such as WIN/DOS, the size of the array is limited to 64kB
// Note that for efficiency reasons, pop() does not clean the released space,
// but the high water mark of the used part is decremented.
// There is no need for iterators, since all the objects can be accessed
// via unsigned int index, and the size of the array is available through
// functions used() and size(). In order to traverse the used part of
// the array, do this:
//      for(i=0; i<myArray.used(); i++){ ...=myArray[i];}
// sort() is based on qsort, and expects the same type of compare function.
// --------------------------------------------------
// When working with an array of pointers, do this:
//   class A;
//   Array<A*> myArray;
// --------------------------------------------------
// Author: Jiri Soukup, Feb.28/95, major modifications Feb.4/97
// --------------------------------------------------

#define hType ArrayContainer<T>

template<class T> class ArrayContainer {
typedef int (*cmpType)(const void *, const void *);
protected:
    static unsigned int maxSz;     // upper limit, automatically derived
    static unsigned int startSz;   // starting size
    unsigned int initMode;  // 0=not, 1=by NULL, 2=by blank
    unsigned int sz,num; // present size, number used
    T *array;
    void init(unsigned int const i,unsigned int const j);
    int sizeChange(const unsigned int k,int mode); // mode=0 by two, =1 exactly
    void start(unsigned int const startSize,unsigned int const ini);
public:
    ArrayContainer(){ start(startSz,0);}
    ArrayContainer(unsigned int const startSize,unsigned int const ini){
                                          start(startSize,ini);}
    virtual ~ArrayContainer(){delete[] array;}
    unsigned int size(void) const{return(sz);}
    unsigned int used(void) const{return(num);} // number of used entries
    T& operator[](const unsigned int k){
              if(k>=sz)sizeChange(k,0); if(k>=num)num=k+1; return array[k];}
    void remove(const unsigned int k);      // remove and shrink remaining part
    void fastRemove(const unsigned int k){ // remove without maintaining order
        if(k<num){array[k]=array[num-1]; num--;}}
    void insert(const unsigned int k,T *t); // insert and expand remaining part
    void push(T* e){if(num>=sz)sizeChange(num,0); array[num]=(*e); num++;}
    T* pop(void){T *e=NULL; if(num>0){e= &(array[num-1]); num--;} return(e);}
    int reduce(){
          return sizeChange(num-1,1);} // reduce the array to its used size
    int cut(const unsigned int k){
          return sizeChange(k,1);} // cut the array to accomodate index k
    int grow(const unsigned int k){
          return sizeChange(k,1);} // grow the array to accomodate index k
    void sort(cmpType cmp);        // sorts the array, using qsort
};

template<class T> unsigned int hType::maxSz=0;
template<class T> unsigned int hType::startSz=8;

// --------------------------------------------------------------
// Remove the entry from the array, and shrink the remaining part
// of the array. This changes 'num' and indexes of all entries
// above this index. 
// --------------------------------------------------------------
template<class T> void hType::remove(const unsigned int k){
    unsigned int j;
    if(k>=num)return;
    for(j=k+1; j<num; j++)array[j-1]=array[j];
    num--;
    init(num,num); // initialize the last entry as unused
}

// --------------------------------------------------------------
// Insert the entry into the array, and expand the remaining part
// of the array. This changes 'used' and indexes of all entries
// above this index. 
// --------------------------------------------------------------
template<class T> void hType::insert(const unsigned int k,T *t){
    unsigned int j;
    if(k>=sz || num==sz)(void)sizeChange(k,0);
    if(k>=num){num=k+1; array[k]=(*t); return;}
    for(j=num-1; j>=k; j--)array[j+1]=array[j];
    array[k]=(*t);
    num++;
}

// --------------------------------------------------------------
// Initialize the array in the specified way:
//  code 0=no initialization, 1=zero initialized, 2=blank initialized
// Remember the instruction, so that the same initialization is used
// when growing the array automatically.
// The first initialized entry has index k, the last has index j.
// --------------------------------------------------------------
template<class T> void hType::init(unsigned int const k,unsigned int const j){
    char *p,*s,c;

    if(k>j)return;
    if     (initMode==0)return; // no initialization
    else if(initMode==1)c='\0';
    else if(initMode==2)c=' ';
    else {
        PTLwarning("wrong ArrayContainer initialization mode=",(void*)initMode);
        return;
    }

    p=(char *)(&(array[k]));
    s=(char *)(&(array[j+1]));
    for(; p!=s; p++) *p=c;
}

// --------------------------------------------------------------
// This function grows the array so that it accomodates the given index.
// Parameter mode controls how the array will grow (or decrease);
// mode=0: increase the size by power of 2 which accomodates the given index;
// mode=1: increase the size to exactly accomodate the given index;
// Warning: mode=1 may reduce the size, and possibly throw away (destroy)
// all objects above the given index.
// Returns 0 if everything OK, returns 1 when problem.
// --------------------------------------------------------------
template<class T> int hType::sizeChange(const unsigned int k,int mode){
    T *a;
    unsigned int j,s,sn;

    if     (mode==0 && k<sz)return 0;
    else if(mode==1 && k==sz-1)return 0;
    if(mode==0){ // proceed multiplying by 2
        for(s=sz; s<=k; s=sn){
            sn=s+s; // faster than to multiply by 2
            if(maxSz>0 && sn>maxSz){s=maxSz; break;}
        }
    }
    else { // go straight for the given size
        s=k+1;
        if(maxSz>0 && s>maxSz)s=maxSz;
    }
    if(s<=k){
        PTLwarning("Arraycon cannot change to size=",(void*)s);
        return(1);
    }
    a=new T[s];
    if(!a){
        PTLwarning("Arraycon, allocation failed when changing to size=",
                                                              (void*)s);
        return(1);
    }
    sn=sz; if(s<sz)sn=s;
    for(j=0; j<sn; j++)a[j]=array[j];
    delete[] array;
    array=a;
    if(sz<s)init(sz,s-1);
    sz=s;
    if(num>sz)num=sz;
    return(0);
}

// --------------------------------------------------------------
// Create array of pointers with the initial size s
// --------------------------------------------------------------
template<class T>
    void hType::start(unsigned int const startSize,unsigned int const ini){
    unsigned long n=65536;
    unsigned int intSz=sizeof(int);
    if(intSz<=2)maxSz=(n/sizeof(void*)); else maxSz=0;
    if(maxSz==0 || startSize<=maxSz)array=new T[startSize]; else array=NULL;
    if(!array){
        PTLerror("allocation, initial allocation failed, size=",
                                                        (void*)startSize);
        return;
    }
    num=0;      // an empty array for the beginning
    sz=startSize;
    startSz=startSize;
    initMode=ini;
    init(0,startSz-1);
}

// --------------------------------------------------------------
// Sort the array by applying qsort with function cmp that compares
// two objects. Only the section of the array which is used will be
// sorted.
// --------------------------------------------------------------
template<class T> void hType::sort(cmpType cmp){
    qsort(array,num,sizeof(T),cmp);
}

// cancel all temporary renaming
#undef hType

#ifdef TEMPLATE_MANAGER
// --------------------------------------------------
// this class is not supposed to be used with Template Manager
// --------------------------------------------------
#endif // TEMPLATE_MANAGER

#endif // ARRAYCON_USED
#define ARRAYCON_USED
