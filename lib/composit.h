// ***********************************************************************
//                       Code Farms Inc.
//          7214 Jock Trail, Richmond, Ontario, Canada, K0A 2Z0
// 
//                     Copyright (C) 1996, 1997
//        This software is subject to copyright protection under
//        the laws of Canada, United States, and other countries.
// ************************************************************************

#ifndef COMPOSITE_USED
#include <collecti.h>
// ---------- composit.g --------------------
// pattern Composite<P,C,i>
// where P is the parent class (for example Picture),
//       C is the child class (for example Graphic)
//       i is an integer index, usually 0
// --------------------------------------------------
// Conditions that must satisfied when using only the template:
//   (1)  P is a subclass of both C and CompositeParent<P,C,i>
//   (2)  C is a subclass of CompositeChild<P,C,i>
//   (3)  Only for global traversing, the user must provide
//        virtual function isComposite for both P and C.
// --------------------------------------------------
// Conditions when using the Pattern Manager:
//   (1)  Pattern(..) statements must be present in the participating
//        classes.
// All the remaining requirements including function isComposite()
// and the inheritance between C and P are provided automatically
// by the Pattern Manager.
// --------------------------------------------------
// Function isComposite() is identical for classes P and C, except
// that it returns 1 for P, and 0 for C. It can be used to detect
// a composite object:
// virtual int P::isComposite(Composite<P,C,i> *c){c=c; return(1);}
// virtual int C::isComposite(Composite<P,C,i> *c){c=c; return(0);}
// The dummy input parameter is essential here for the parametrization
// in situations when several composite patterns are used simultaneously.
// --------------------------------------------------
// Example when using pure templates:
//   Class Graphic : public CompositeChild<Picture,Graphic,0> {
//       ...
//   public:
//       virtual int isComposite(Composite *c){c=c; return(0);}
//       ...
//  };
//   Class Picture : public Graphic,
//                   public CompositeParent<Picture,Graphic,0> {
//       ...
//   public:
//       virtual int isComposite(Composite *c){c=c; return(1);}
//       ...
//  };
// --------------------------------------------------
// Example when using Template Manager.
// Note that even the inheritance between Graphic and Picture is
// supplied automatically:
//   Class Graphic : Pattern(Graphic) {
//       ...
//  };
//   Class Picture : Pattern(Picture) {
//       ...
//  };
// --------------------------------------------------
// Author: Jiri Soukup, Feb.8/95
// --------------------------------------------------

// the following defines are not macros, they only introduce
// shorter names - long names make the code difficult to read
#define  pType CompositeParent<P,C,i>
#define  cType CompositeChild<P,C,i>
#define  iType CompositeIterator<P,C,i>
#define  oType Composite<P,C,i>


template<class P,class C,int i> class Composite;
template<class P,class C,int i> class CompositeIterator;

template<class P,class C,int i> class CompositeChild :
                               public CollectionChild<P,C,i>{
friend class oType;
friend class iType;
public:
    virtual ~CompositeChild(){};
};

template<class P,class C, int i> class CompositeParent :
                                     public CollectionParent<P,C,i>{
friend class oType;
friend class iType;
public:
    virtual ~CompositeParent(){}
};

template<class P,class C, int i> class Composite :
                               public Collection<P,C,i>{
typedef C* (*TravFun)(C*,void *);
public:
    // all methods of Collection are inherited
    void dissolve(cType *c);
    C *depthFirst(cType *c,TravFun f,void *v);
};

// This iterator traverses one layer of the hierarchy.
// It can be used in multiple loops. For tree traversal, use
// it recursively.
template<class P,class C, int i> class CompositeIterator :
                               public CollectionIterator<P,C,i>{
public:
    C* begin(const pType *p){
                 return (C*)(((CollectionIterator<P,C,i> *)this)->begin(p));}
    C* const operator++(){
                      return (C*)(++(*((CollectionIterator<P,C,i> *)this)));}
    C* const operator--(){
                      return (C*)(--(*((CollectionIterator<P,C,i> *)this)));}
};

// ------------------------------------------------------------------
// Dissolve the entire subtree which is under p. This is a recursive
// function, which relies on the callback function isComposite()
// which must be supplied by the user, and which returns 1 for
// a composite object.
// ------------------------------------------------------------------
template<class P,class C,int i> void oType::dissolve(cType *c){
    CompositeIterator<P,C,i> it;
    cType *s; pType *p;

    if(((C*)c)->isComposite(this)==0)return; // no subtree
    p=(P*)c;
    for(s=it.begin(p); s; s= ++it){
        if(((C*)s)->isComposite(this)) dissolve(s);
        remove(p,s);
    }
}

// ------------------------------------------------------------------
// Starting from the given node, traverse the hierarchy depthFirst,
// and apply function f to every node. If f returns 1, stop traversing
// and exit. 
// Returns: 1=exit before traversing all nodes
//          0=all nodes were traversed
// ------------------------------------------------------------------
template<class P,class C,int i> 
                 C* oType::depthFirst(cType *c,TravFun f,void *v){
    CompositeIterator<P,C,i> it;
    cType *s; pType *p; C *cRet;

    cRet=(*f)((C*)c,v);
    if(cRet)return cRet;

    if(((C*)c)->isComposite(this)==0)return NULL; // no subtree
    p=(P*)c;
    for(s=it.begin(p); s; s= ++it){
        cRet=depthFirst(s,f,v);
        if(cRet)return cRet;
    }
    return NULL;
}

// cancel all temporary renaming
#undef pType
#undef cType
#undef iType
#undef oType

#ifdef TEMPLATE_MANAGER
// --------------------------------------------------
// Include sections - needed only when the Template Manager is used
#define CompositeInherit1(id,par,chi,i) \
    public chi, public CompositeParent<par,chi,i>
#define CompositeMember1(id,par,chi,i)      \
    public:                                      \
        virtual int isComposite(Composite<par,chi,i> *c){/*c=c;*/ return 1;} \
    private:
#define CompositeInherit2(id,par,chi,i) public CompositeChild<par,chi,i>
#define CompositeMember2(id,par,chi,i)     \
    public:                                      \
        virtual int isComposite(Composite<par,chi,i> *c){/*c=c;*/ return 0;} \
    private:
// --------------------------------------------------
#endif  // TEMPLATE_MANAGER
#endif  // COMPOSITE_USED
#define COMPOSITE_USED
