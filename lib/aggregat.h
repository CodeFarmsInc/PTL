// ***********************************************************************
//                       Code Farms Inc.
//          7214 Jock Trail, Richmond, Ontario, Canada, K0A 2Z0
// 
//                     Copyright (C) 1996
//        This software is subject to copyright protection under
//        the laws of Canada, United States, and other countries.
// ************************************************************************

#ifndef AGGREGATE_USED
#include <collecti.h>
// ---------- aggregat.g --------------------
// pattern Aggregate<P,C,i>
// where P is the parent class (for example Department),
//       C is the child class (for example Employee)
//       i is an integer index, usually 0
// --------------------------------------------------
// Conditions:
//  - P must inherit from AggregateParent<P,C,i>
//  - C must inherit from AggregateChild<P,C,i>
//  When using the Template Manager, both these conditions are
//  automatically satisfied.
// --------------------------------------------------
// Example:
// Class Employee : public AggregateChild<Department,Employee,0> {
//     ...
//};
// Class Department : public AggregateParent<Department,Employee,0> {
//     ...
//};
// --------------------------------------------------
// Author: Jiri Soukup, Feb.19/95, last update Jan.30/97
// --------------------------------------------------

// the following defines are not macros, they only introduce
// shorter names - long names make the code difficult to read
#define pType AggregateParent<P,C,i>
#define cType AggregateChild<P,C,i>
#define iType AggregateIterator<P,C,i>
#define oType Aggregate<P,C,i>

#define pTypeC CollectionParent<P,C,i>
#define cTypeC CollectionChild<P,C,i>
#define iTypeC CollectionIterator<P,C,i>
#define oTypeC Collection<P,C,i>


template<class P,class C,int i> class Aggregate;
template<class P,class C,int i> class AggregateIterator;

#ifdef PTL_CANNOT_INITIALIZE
template<class P,class C, int i> class AggregateParent : public pTypeC {
#else

template<class P,class C, int i=0> class AggregateParent : public pTypeC {
#endif
friend class oType;
friend class iType;
public:
    AggregateParent():pTypeC(){}
    virtual ~AggregateParent(){}
};

#ifdef PTL_CANNOT_INITIALIZE
template<class P,class C,int i> class AggregateChild :
#else

template<class P,class C,int i=0> class AggregateChild :
#endif
                               public cTypeC {
friend class oType;
friend class iType;
    AggregateParent<P,C,i> *parent; 
public:
    AggregateChild():cTypeC(){parent=NULL;}
    virtual ~AggregateChild(){ if(parent)
        PTLerror("destroying child associated with an Aggregate=",this);}
};

#ifdef PTL_CANNOT_INITIALIZE
template<class P,class C, int i> class Aggregate :
#else

template<class P,class C, int i=0> class Aggregate :
#endif
                               public oTypeC{
typedef int (*cmpType)(const C *,const C *);
    void merge(cType *s,cType *t,pType *ps,pType *pt) const;
    void split(cType *s,cType *t,pType *ps,pType *pt) const;
public:
    pType *parent(cType *c) const{return c->parent;}
    void addTail(pType *p,cType *c) const{
                   c->parent=p; ((oTypeC *)this)->addTail(p,c);}
    void addHead(pType *p,cType *c) const{
                   c->parent=p; ((oTypeC *)this)->addHead(p,c);}
    void addSorted(pType *p,cType *c,cmpType cmp) const{
                   c->parent=p; ((oTypeC *)this)->addSorted(p,c,cmp);}
    void setTail(cType *c) const{
             pType* p=c->parent; ((oTypeC *)this)->setTail(p,c);}
    void setHead(cType *c) const{
             pType* p=c->parent; ((oTypeC *)this)->setHead(p,c);}
    void insert(cType *c,cType *x) const{ pType* p=c->parent;
                   x->parent=p; ((oTypeC *)this)->insert(p,c,x);}
    void append(cType *c,cType *x) const{ pType* p=c->parent;
                   x->parent=p; ((oTypeC *)this)->append(p,c,x);}
    void remove(cType *c) const{ 
                   ((oTypeC *)this)->remove(c->parent,c); c->parent=NULL;}
    void merge(cType *s,cType *t) const {
                   pType* ps=s->parent; pType* pt=t->parent; merge(s,t,ps,pt);}
    void split(cType *s,cType *t,pType *pt) const {
                   pType* ps=s->parent; merge(s,t,ps,pt);}
};

// This iterator is the same as for the collection
#ifdef PTL_CANNOT_INITIALIZE
template<class P,class C, int i> class AggregateIterator :
                               public iTypeC{};
#else

template<class P,class C, int i=0> class AggregateIterator :
                               public iTypeC{};
#endif

// ------------------------------------------------------------------
// Merge and split provide safe checking to prevent accidental mixup
// between split and merge for the collection. Also, after the operation
// is finished, the parent pointer must be updated.
// ------------------------------------------------------------------
template<class P,class C, int i> 
          void oType::merge(cType *s,cType *t,pType *ps,pType *pt) const{
    if(s->parent!=ps || t->parent!=pt || ps==pt){
        PTLwarning("parent/child relations, Aggregate merge not performed,",s);
        return;
    }
    {
        iTypeC it; cType *c;
        for(c=it.begin(ps); c; c= ++it){ c->parent=ps;}
        ((oTypeC *)this)->merge(s,t,ps,pt);
    }
}

template<class P,class C, int i> 
              void oType::split(cType *s,cType *t,pType *ps,pType *pt) const{
    if(s->parent!=ps || t->parent!=ps){
        PTLwarning("parent/child relations, Aggregate split not performed,",s);
        return;
    }
    {
        ((oTypeC *)this)->split(s,t,ps,pt); cType *c;
        iTypeC it;
        for(c=it.begin(pt); c; c= ++it){ c->parent=pt;}
    }
}

#ifdef TEMPLATE_MANAGER
// --------------------------------------------------
// Include sections - needed only when the Template Manager is used
#define AggregateInherit1(id,par,chi,i) public AggregateParent<par,chi,i>
#define AggregateMember1(id,par,chi,i)
#define AggregateInherit2(id,par,chi,i) public AggregateChild<par,chi,i>
#define AggregateMember2(id,par,chi,i) 
// --------------------------------------------------
#endif  // TEMPLATE_MANAGER
// cancel all temporary renaming
#undef pType
#undef cType
#undef iType
#undef oType
#undef pTypeC
#undef cTypeC
#undef iTypeC
#undef oTypeC
#endif  // AGGREGATE_USED
#define AGGREGATE_USED
