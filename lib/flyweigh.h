// ***********************************************************************
//                       Code Farms Inc.
//          7214 Jock Trail, Richmond, Ontario, Canada, K0A 2Z0
// 
//                     Copyright (C) 1996
//        This software is subject to copyright protection under
//        the laws of Canada, United States, and other countries.
// ************************************************************************

#ifndef FLYWEIGHT_USED
#include <arraycon.h>
// ---------------------- flyweigh.g ---------------------------------
// pattern Flyweight<K,H,C,S>
// where K is the Keeper type (for example a Line of letters)
//       H is the HeavyWeight type (for example a Character master)
//       C is the calculated state (for example x,y position, font)
//       S is the stored state (for example character width)
//       i   permits several Flyweight paterns for the same set of classes.
// Either of the two states may not be readily available, and may
// be derived from other information.
// The LightWeight objects are assumed to be integers - whether 1,2, or more
// bytes depends entirely on the application.
// --------------------------------------------------------------------
// Conditions that must satisfied when using only the template:
//   (1) Class K must be derived from
//                        FlyweightKeeper<K,H,C,S>
//   (2) Class H must be derived from
//                        FlyweightHeavy<K,H,C,S>
//   (3) Both K and H must be friends of Flyweight<K,H,C,S>
// It is recommended that K be a friend of C, and H be a friend of S.
// --------------------------------------------------------------------
// Conditions when using the Pattern Manager:
//                        N/A
// K is automatically a friend of C, and H is a friend of S
// --------------------------------------------------------------------
// Note that C and S may use static variables if you don't want
// more than one instance of the state object.
//
// Classes FlyweightKeeper and FlyweightHeavy are derived
// from C and S.
// Both K and H can get a pointer to its own state (and update
// the state without copying it) simply by calling getFlyweightState()
// This is a virtual function which can be overloaded by both K and H.
//
// K can call functions of H only through Flyweight::fun().
// This function is pivotal for the operation of this implementation.
// It assumes that H has one or more functions of the type
//      int f(C*) const;
// which must be called from the K environment without having 
// a direct access to H. K does not have access to H by pointer;
// that could lead to an error, because Hs change their positions 
// when the array re-allocates itself.
// --------------------------------------------------------------------
// Example of using pure templates:
//   Class LineState {
//       int x,y;
//       float scale;
//   };
//   class Line : public 
//    FlyweightKeeper<Line,Character,LineState,CharacterState> {
//   friend class Flyweight<Line,Character,LineState,CharacterState>;
//       ...
//   public:
//       ...
//  };
//  Class CharacterState {
//      int width;
//  };
//  Class Character {
//   FlyweightHeavy<Line,Character,LineState,CharacterState> {
//  friend class Flyweight<Line,Character,LineState,CharacterState>;
//      ...
//  public:
//      ...
// };
//  Flyweight<Line,Character,LineState,CharacterState> flw;
// --------------------------------------------------
// Example when using Template Manager:
// Class LineState {
//     int x,y;
//     static float scale;
// };
// Class Line : Pattern(Line) {
//     ...
// public:
//     ...
// };
// Class CharacterState {
//     int width;
//     static int height;
// };
// Class Character : Pattern(Character) {
//     ...
// public:
//     ...
// };
// pattern Flyweight<Line,Character,LineState,CharacterState> flw;
// --------------------------------------------------
// Author: Jiri Soukup, Feb.8/95, update Jul.22/97
// --------------------------------------------------

// the following defines are not macros, they only introduce
// shorter names - long names make the code difficult to read
#define kType FlyweightKeeper<K,H,C,S>
#define hType FlyweightHeavy<K,H,C,S>
#define fType Flyweight<K,H,C,S>
#define aType ArrayContainer<H*>


template<class K,class H,class C,class S> class Flyweight;

template<class K,class H,class C,class S> 
                            class FlyweightHeavy : protected S {
friend class fType;
public:
    virtual S *getFlyweightState(void){return (S *)this;}
    virtual ~FlyweightHeavy(){};
};

template<class K,class H,class C,class S> 
                            class FlyweightKeeper : protected C {
friend class fType;
public:
    virtual C *getFlyweightState(void){return (C *)this;}
    virtual ~FlyweightKeeper(){};
};

template<class K,class H,class C,class S> class Flyweight :
                               public aType{
public:
    Flyweight() : aType(10,1){}
    Flyweight(const unsigned int sz) : aType(sz,1) {} // 1=init by 0
    virtual ~Flyweight(){};
    void add(H *const ip,const unsigned int k);
    void force(H *const ip,const unsigned int k){remove(k); add(ip,k);}
    void remove(const int k){(void)(((aType*)this)->remove(k));}
                                                       // remove entry
    void remove(const H *ip); // search for this entry and remove it
    void cleanHeavy(void); // destroy all intrinsic objects
    S *getState(const unsigned int k);
typedef int (H::*funType)(C*);
    int fun(const unsigned int k, funType f, K *ep) const;
};

// ------------------------------------------------------------------
// Add the given H to the Flyweight internal representation at
// position k. If the position is occupied, do nothing, and issue
// en error message.
// ------------------------------------------------------------------
template<class K,class H,class C,class S> 
                void fType::add(H *const ip,const unsigned int k){
    H *kp;

    kp=(*this)[k];
    if(kp){
        PTLwarning("Flyweight adding to an occupied position=",(void*)k);
        return;
    }
    (*this)[k]=ip;
}

// ------------------------------------------------------------------
// Remove a given H from its Flyweight representation. This is 
// a relatively inefficient operation, which involves a linear search
// for this object, then removing its pointer from the internal table.
// ------------------------------------------------------------------
template<class K,class H,class C,class S> 
                              void fType::remove(const H *ip){
    unsigned int k,s;
    H *kp;

    if(!ip)return;
    s=((aType*)this)->size();
    for(k=0; k<s; k++){
        kp=(*this)[k];
        if(ip==kp)break;
    }
    if(k<s)remove(k);
}

// ------------------------------------------------------------------
// Destroy all heavy objects recorded in the Flyweight, but
// keep the Flyweight object and its internal table for possible reuse.
// All entries of the internal table will be NULL after this.
// ------------------------------------------------------------------
template<class K,class H,class C,class S> 
                                  void fType::cleanHeavy(void) {
    unsigned int k,s;
    H *kp;

    s=((aType*)this)->size();
    for(k=0; k<s; k++){
        kp=(*this)[k];
        if(kp){delete kp; (*this)[k]=NULL;}
    }
}

// ------------------------------------------------------------------
// Get the stored state for the object associated with key k.
// This method will be called typically by K.
// Return NULL if this key is not used.
// ------------------------------------------------------------------
template<class K,class H,class C,class S> 
                          S * fType::getState(const unsigned int k){
    H* kp=(*this)[k]; 
    if(!kp)return(NULL);
    return ((hType*)kp)->getFlyweightState();
}

// ------------------------------------------------------------------
// Flyweight general function which permits to invoke those
// functions of H which need information about the state of K.
// This method will be called typically by K.
// The function returns the same value as the H function.
// When attempting to operate on an empty entry, no action is taken
// an error message is printed, and the return is (-1).
// ------------------------------------------------------------------
template<class K,class H,class C,class S> 
int Flyweight<K,H,C,S>::fun(
            const unsigned int k, funType f, K *ep) const {
    H* ip=(*((aType*)this))[(unsigned int)k]; 
    C* es=ep->getFlyweightState();
    if(!ip || !es){
        PTLwarning("Flyweight calling function for empty key k=",(void*)k);
        return(-1);
    }
    return ((*ip).*f)(es);
}

// cancel all temporary renaming 
#undef hType
#undef kType
#undef fType
#undef aType

#ifdef TEMPLATE_MANAGER
// --------------------------------------------------
// dummy base classes to make the checking of the patterns consistent
template<class C> class FlyweightKeeperBase{
public:
    virtual C *getFlyweightState(void)=0;
};
template<class S> class FlyweightHeavyBase{
public:
    virtual S *getFlyweightState(void)=0;
};

// Include sections - needed only when the Template Manager is used
#define FlyweightInherit1(id,exo,ino,exs,ins) \
    public FlyweightKeeper<exo,ino,exs,ins>
#define FlyweightMember1(id,exo,ino,exs,ins)

#define FlyweightInherit2(id,exo,ino,exs,ins) \
    public FlyweightHeavy<exo,ino,exs,ins>
#define FlyweightMember2(id,exo,ino,exs,ins)

#define FlyweightInherit3(id,exo,ino,exs,ins) \
    public FlyweightKeeperBase<exs>
#define FlyweightMember3(id,exo,ino,exs,ins) \
friend class exo; \
public: \
    virtual exs *getFlyweightState(void){return this;}

#define FlyweightInherit4(id,exo,ino,exs,ins) \
    public FlyweightHeavyBase<ins>
#define FlyweightMember4(id,exo,ino,exs,ins) \
friend class ino; \
public: \
    virtual ins *getFlyweightState(void){return this;}
// --------------------------------------------------
#endif  // TEMPLATE_MANAGER
#endif  // FLYWEIGHT_USED
#define FLYWEIGHT_USED
