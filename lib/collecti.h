// ***********************************************************************
//                       Code Farms Inc.
//          7214 Jock Trail, Richmond, Ontario, Canada, K0A 2Z0
// 
//                     Copyright (C) 1996
//        This software is subject to copyright protection under
//        the laws of Canada, United States, and other countries.
// ************************************************************************

#ifndef COLLECTION_USED
#include <mgr.h>
// ---------- collecti.g --------------------
// pattern Collection<P,C,i>
// This is a collection based on a doubly-linked intrusive list,
// where P is the parent class (for example Department),
//       C is the child class (for example Employee)
//       i is an integer index, usually 0
// --------------------------------------------------
// Conditions that must satisfied when using this pattern:
//   (1)  P is a subclass of CollectionParent<P,C,i>
//   (2)  C is a subclass of CollectionChild<P,C,i>
// When using the PatternManager, this happens automatically.
// --------------------------------------------------
// Author: Jiri Soukup, Feb.6/95, last update Jan.30/97
// --------------------------------------------------

// the following defines are not macros, only to use shorter
// names - long names make the code difficult to read
#define pType CollectionParent<P,C,i>
#define cType CollectionChild<P,C,i>
#define iType CollectionIterator<P,C,i>
#define oType Collection<P,C,i>

template<class P,class C,int i> class Collection;
template<class P,class C,int i> class CollectionIterator;

#ifdef PTL_CANNOT_INITIALIZE
template<class P,class C,int i> class CollectionChild {
#else

template<class P,class C,int i=0> class CollectionChild {
#endif
friend class oType;
friend class iType;
protected:
    CollectionChild *next;
    CollectionChild *prev;
public:
    CollectionChild(){next=prev=NULL;}
    virtual ~CollectionChild(){if(next||prev)
        PTLerror("destroying CollectionChild without disconnecting it=",this);}
};

#ifdef PTL_CANNOT_INITIALIZE
template<class P,class C, int i> class CollectionParent {
#else

template<class P,class C, int i=0> class CollectionParent {
#endif
friend class oType;
friend class iType;
protected:
    cType *tail;
public:
    CollectionParent(){tail=NULL;}
    virtual ~CollectionParent(){if(tail) 
        PTLerror("destroying CollectionParent without disconnecting it=",this);}
};

#ifdef PTL_CANNOT_INITIALIZE
template<class P,class C, int i> class Collection {
#else

template<class P,class C, int i=0> class Collection {
#endif
    typedef int (*cmpType)(const C *,const C *);
    void splitMerge(cType *s,cType *t,pType *ps,pType *pt) const;
    cType *mergeSublists(cType *s,cType *t,cmpType cmp) const;
public:
    cType *head(const pType *p) const{
                     cType *c=p->tail; if(c)c=c->next; return c;}
    cType *tail(const pType *p) const{return p->tail;}
    cType *next(const cType *c) const{return c->next;}
    cType *prev(const cType *c) const{return c->prev;}
    void setTail(pType *p,cType *c) const{p->tail=c;}
    void setHead(pType *p,cType *c) const{p->tail=c->prev;}
    void addTail(pType *p,cType *c) const{addHead(p,c); setTail(p,c);}
    void addHead(pType *p,cType *c) const;
    void insert(pType *p,cType *c,cType *x) const;
    void append(pType *p,cType *c,cType *x) const;
    void remove(pType *p,cType *c) const;
    void merge(cType *s,cType *t,pType *ps,pType *pt) const{
                                               splitMerge(s,t,ps,pt);}
    void split(cType *s,cType *t,pType *ps,pType *pt) const{
                                               splitMerge(s,t,ps,pt);}
    void merge(pType *ps,pType *pt){merge(ps->tail,pt->tail,ps,pt);}
    void sort(pType *p,cmpType cmp) const;
    void addSorted(pType *p,cType *c,cmpType cmp) const;
    int count(const pType* p) const;
};

#ifdef PTL_CANNOT_INITIALIZE
template<class P,class C, int i> class CollectionIterator {
#else

template<class P,class C, int i=0> class CollectionIterator {
#endif
protected:
    cType *tail; 
    cType *fwd; // next forward
    cType *bwd; // next backward
public:
    CollectionIterator(){tail=fwd=bwd=NULL;}
    C* begin(const pType *p); // starts iterator from the head
    C* end(const pType *p);   // starts iterator from the tail
    C* const operator++();
    C* const operator--();
    void * prt(void){return (void*)tail;}
};

template<class P,class C,int i> C* iType::begin(const pType *p){
    cType *c;

    if(p)tail=p->tail; else tail=NULL;
    if(tail)c=tail->next; else c=NULL;
    if(c==tail)fwd=NULL; else fwd=c->next; 
    bwd=NULL;
    return((C*)c);
}

template<class P,class C,int i> C* iType::end(const pType *p){
    cType *c;

    if(p)tail=p->tail; else tail=NULL;
    c=tail;
    if(tail)bwd=tail->prev; else bwd=NULL;
    if(bwd==tail)bwd=NULL;
    fwd=NULL;
    return((C*)c);
}

template<class P,class C,int i> C* const iType::operator++(){ 
    cType *c;

    if(!tail)return NULL;
    if(fwd)c=fwd;  else {fwd=bwd=NULL; return NULL;}
    if(c==tail)fwd=NULL; else fwd=c->next;
    bwd=c->prev; if(bwd==tail)bwd=NULL;
    return((C*)c);
}

template<class P,class C,int i> C* const iType::operator--(){
    cType *c;

    if(!tail)return NULL;
    if(bwd)c=bwd;  else {fwd=bwd=NULL; return NULL;}
    if(c==tail)fwd=NULL; else fwd=c->next;
    bwd=c->prev; if(bwd==tail)bwd=NULL;
    return((C*)c);
}

// Add the new child to the head of the collection
template<class P,class C,int i> void oType::addHead(pType *p,cType *c) const{
    cType *a,*b;
    if(c->next)
        PTLwarning("add() not executed, object already in the collection=",c);
    else {
        if(p->tail){
            a=p->tail; b=a->next;
            c->next=b;  c->prev=a;
            a->next=c;  b->prev=c;
        }
        else {
            p->tail=c; c->next=c; c->prev=c;
        }
    }
}

// Add the new child so that the order is maintained
// The algorithm walks from the tail, so that when adding items
// in the correct order, it is very efficient.
template<class P,class C,int i> 
             void oType::addSorted(pType *p,cType *c,cmpType cmp) const{
    cType *a;
    int n;

    if(c->next){
        PTLwarning("add() not executed, object already in the collection=",c);
    }
    else {
        if(p->tail){
            n=(*cmp)((C*)(p->tail), (C*)c);
            if(n <= 0){
                addTail(p,c); 
            }
            else {
                for(a=p->tail; ; ){
                    if((*cmp)((C*)a, (C*)c) <= 0){append(p,a,c); break;}
                    a=a->prev;
                    if(a==p->tail){addHead(p,c); break;}
                }
            }
        }
        else {
            p->tail=c; c->next=c; c->prev=c;
        }
    }
}

// Insert x before c; if c is a head, x will become a new head.
template<class P,class C,int i> 
                    void oType::insert(pType *p,cType *c,cType *x) const{
    cType *a;

    if(x->next)
      PTLwarning("object not inserted, already in the collection=",x);
    else if(c->next==NULL || p->tail==NULL)
      PTLwarning("cannot insert before object not in the collection=",c);
    else {
        a=c->prev; x->prev=a; x->next=c; a->next=x; c->prev=x;
    }
}

// Append x after c; if c is a tail, x will become a new head.
template<class P,class C,int i>
                    void oType::append(pType *p,cType *c,cType *x) const{
    cType *b;

    if(x->next)
        PTLwarning("cannot append object, already in a collection=",x);
    else if(c->next==NULL || p->tail==NULL)
        PTLwarning("cannot append after object not in a collection=",c);
    else {
        b=c->next; x->prev=c; x->next=b; c->next=x; b->prev=x;
        if(c==p->tail)p->tail=x;
    }
}

// Remove c from the collection
template<class P,class C,int i> void oType::remove(pType *p,cType *c) const{
    cType *a,*b;

    a=c->prev; b=c->next;
    if(!(p->tail) || !a || !b)
        PTLwarning("corrupted collection, cannot remove=",c);
    else {
        if(a==c)p->tail=NULL;
        else {
            if(p->tail==c)p->tail=a;
            a->next=b; b->prev=a;
        }
        c->next=c->prev=NULL;
    }
}

// ---------------------------------------------------------------
// This method provides two functions:
//  (1) merging two collections
//  (2) splitting a collection into two
// As shown in the following diagram, one algorithm can do both jobs; 
// We only disconnect links s-u and t-v, and connect s-v and t-u.

//             merging                 splitting
//
//     ..<...v....t..<..              ......<.....
//     .     |    |    .              .          .
//     ..>...s....u..>..              ....v..t....
//                                        |  |
//                                    ....s..u....
//                                    .          .
//                                    ......<.....

// When s and t are in the same collection merge(s,t,ps,pt) splits
// the original collection into the original collection ps (where s
// becomes the head), and pt which contains t.
// Note that an unused pt must be provided externally.

// When s and t are in different collections, merge(s,t,ps,pt) will 
// merge the collections, with s as teh head of ps.
// The collection on pt will be discarded.

// When s==t, no action is taken.
// No check is made for s being an element of ps or t being an element of pt.
// ---------------------------------------------------------------
template<class P,class C,int i>
          void oType::splitMerge(cType *s,cType *t,pType *ps,pType *pt) const{
    cType *u,*v;

    if(s==t)return; // same objects, no action
    u=s->next; v=t->next;
    if(!ps || !pt || !u || !v){
        PTLwarning("cannot merge/split collections=",s);
        return;
    }
    // re-linking s-u, t-v to s-v, t-u
    t->next=u; v->prev=s;
    s->next=v; u->prev=t;
    // reset tail on both collections
    ps->tail=s->prev;
    if(pt->tail)pt->tail=NULL; else pt->tail=s->prev;
}

// ---------------------------------------------------------------
// Sort the collection using the recursive merging of pre-sorted lists.
// This algorithm is very efficient, comparable to qsort() for arrays.
// Note that the algorithm is implemented without calling a recursive function.
//
// Algorithm:
// Pointer 'next' is temporarily used to convert the list into many
// NULL ending sorted sublists, originally each having at most 2 items.
// Pointer 'prev' is temporarily used to link heads of the sublists.
// By repeated merging of adjacent lists, the entire list is sorted.
// After this, the NULL ending list of 'next' is converted into a ring,
// and pointer 'prev' is properly set in a single pass.
// ---------------------------------------------------------------
template<class P,class C,int i>
          void oType::sort(pType *p,cmpType cmp) const{
    cType *a,*b,*c,*aNext,*bNext,*sub,*last,*pTail;

    pTail=p->tail;
    if(pTail==NULL || pTail==pTail->next)return; // 1 or 0 items on list

    // Break the list into sorted sublists 2 long or longer
    // the first list will start from sub, each sublist NULL ending
    // and singly linked with 'next', heads of all lists linked together
    // with 'prev' and NULL ending. The last sublist can have a single object.
    for(sub=last=NULL, a=pTail->next; a; a=aNext){
        if(a==pTail){ // single last item
            if(sub==NULL)sub=a; else last->prev=a;
            last=a; a->prev=a->next=NULL;
            break;
        }
        for(b=a, c=b->next; b!=pTail; b=c, c=c->next){
            if((*cmp)((C*)b, (C*)c)>0)break;
        }
        if(a==b){ // less then 2 items, exchange a and b
            b=a->next; // case of a single item at the end is already covered
            if(b==pTail)aNext=NULL; else aNext=b->next; // before changing b
            if(sub==NULL)sub=b; else last->prev=b;
            last=b; b->prev=NULL; b->next=a; a->next=NULL;
        }
        else { // treat a-b as a new sublist
            if(b==pTail)aNext=NULL; else aNext=b->next; // before changing b
            if(sub==NULL)sub=a; else last->prev=a;
            last=a; a->prev=NULL; b->next=NULL;
        }
    }
        
    // merge the subsorted lists until only one list is left
    while(sub->prev){
        for(a=sub, b=sub->prev, last=NULL; a; a=aNext, b=bNext){
            if(b==NULL){ // odd list left at the end
                if(last==NULL)last=a; else last->prev=a;
                a->prev=NULL;
                break;
            }
            aNext=b->prev;
            if(aNext)bNext=aNext->prev; else bNext=NULL;
            if(b) c=mergeSublists(a,b,cmp); else  c=a;
            if(last==NULL){sub=c; last=c;} else last->prev=c;
            c->prev=NULL;
        }
    }

    // convert pointers 'next' and 'prev' to normal use
    for(a=sub, b=a->next; b; a=a->next, b=b->next){ b->prev=a;}
    p->tail=a; a->next=sub; sub->prev=a;
}

// ---------------------------------------------------------------
// Auxilliary function to merge sorted sublists. This function uses
// pointers 'next' and 'prev' in a special way (see the algorithm 
// description for sort()), and is not for public use.
// ---------------------------------------------------------------
template<class P,class C,int i>
          cType* oType::mergeSublists(cType *s,cType *t,cmpType cmp) const{
    cType *ss,*tt,*beg,*last;

    // start from the smaller point
    if((*cmp)((C*)s, (C*)t)<=0){beg=s; ss=s->next; tt=t;}
    else            {beg=t; ss=t->next; tt=s;}
    for(last=beg; ss || tt; ){
      if(ss==NULL)                     {last->next=tt; last=tt; tt=tt->next;}
      else if(tt==NULL)                {last->next=ss; last=ss; ss=ss->next;}
      else if((*cmp)((C*)ss,(C*)tt)<=0){last->next=ss; last=ss; ss=ss->next;}
      else                             {last->next=tt; last=tt; tt=tt->next;}
    }
    return beg;
}

// ---------------------------------------------------------------
// This is not an efficient function, must traverse the list.
// Storing the count on the Parent would be a waste of space.
// ---------------------------------------------------------------
template<class P,class C,int i> int oType::count(const pType *p) const{
    iType it; cType *c; int n=0;
    for(c=it.begin(p); c; c= ++it) n++;
    return n;
}

// cancel all temporary renaming
#undef pType
#undef cType
#undef iType
#undef oType

#ifdef TEMPLATE_MANAGER
// --------------------------------------------------
// include sections - needed only when the Template Manager is used
#define CollectionInherit1(id,par,chi,i) public CollectionParent<par,chi,i>
#define CollectionInherit2(id,par,chi,i) public CollectionChild<par,chi,i>
#define CollectionMember1(id,par,chi,i)
#define CollectionMember2(id,par,chi,i)
// --------------------------------------------------
#endif // TEMPLATE_MANAGER

#endif // COLLECTION_USED
#define COLLECTION_USED
