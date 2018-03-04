// ***********************************************************************
//                       Code Farms Inc.
//          7214 Jock Trail, Richmond, Ontario, Canada, K0A 2Z0
// 
//                     Copyright (C) 1996
//        This software is subject to copyright protection under
//        the laws of Canada, United States, and other countries.
// ************************************************************************
//
//                  FSM = FINITE STATE MACHINE
// ************************************************************************
//   This is the implementation style described by J.Coplien in his
//   article "Curiously reccuring template patterns", C++ Report,
//   Vol.7/No.2, Feb.95, pp.24-27.
//   Apparently, the idea was independently invented by Lorraine Juhl,
//   John Barton and Lee Nackman, Tim Budd, Paul Lukas, and Ralph Kolewe.
//   Instead of creating a special pattern class, the user's FSM inherits
//   what is the equivalent of the pattern class. This makes the design
//   somewhat convoluted (a class inheriting a template where the class is
//   one of template parameters), but the user interface is elegant.
// ************************************************************************

#ifndef FSM_USED
#include <arraycon.h>
// ------------------------- fsm.g ---------------------------------
// The implementation supports dynamic creation and changes of the FSM
// but, at the same time, provides a fast operation for the established logic.
//
// At each transtion, two things happen: First, the function specified
// for this combination of input/state is executed. If the function
// returns a new state, FSM transfers to this state. If the function returns
// NULL, FSM uses the internal table (state,input)->input to decide about
// the next state.
//
// The logic of the FSM is established by calls to 
//      addTrans(input,oldState,newState,fun), 
// where input must always be given (use a special class like NoInput if
//             the FSM may transfer to the next state without any input given);
//       oldState is NULL if this transition applies to any old state,
//       newState is NULL when no change of the state implies,
//       fun is a member function of FSM to be called, use NULL when no
//             function call is required.
// --------------------------------------------------------------------
// Conditions that must be satisfied when using pure templates:
//   (1) All application states must be derived from a common base class,
//       S, which itself is derived from FSMstate<F,S,I,T,D>.
//   (2) All application input stimulae must be derived from a common base
//       class, which itself is derived from FSMinput<F,S,I,T,D>.
//   (3) The application finite state machine F must be derived from
//       FSM<F,S,I,T,D>.
//   (4) Functions to be executed for individual transitions must be
//       static functions of F.
// --------------------------------------------------------------------
// Conditions that must satisfied when using Template Manager
// for FSM<F,S,I,T,D>:
//   (1) All application states must be derived from S.
//   (2) All application input stimulae must be derived from I.
//   (3) Functions to be executed for individual transitions must be
//       static functions of F.
// --------------------------------------------------------------------
// Author: Jiri Soukup, Jul.3/96, major overhaul Feb.27/97
// --------------------------------------------------------------------
//   Meaning of template parameters:
//   F = application finite state machine
//   S = application state class
//   I = application input class
//   T = type of internal table entry (unsigned char, unsigned short, ...)
//   D = integer id as a provision for multiple patterns
//   
//   Type T must be chosen so that it can express the integer value of
//   MAX(numInputs,numStates,numFuncts), and must be 'unsigned'.
// --------------------------------------------------

#define fType FSM<F,S,I,T,D>
#define sType FSMstate<F,S,I,T,D>
#define iType FSMinput<F,S,I,T,D>
template<class F,class S,class I,class T,int D> class FSM;

#ifdef PTL_CANNOT_INITIALIZE
template<class F,class S,class I,class T,int D> class FSMstate {
#else

template<class F,class S,class I,class T,int D=0> class FSMstate {
#endif
friend class fType;
    T index; // internal (index+1) for fast reference, 0 means unused
public:
    FSMstate(){index=0;}
};

#ifdef PTL_CANNOT_INITIALIZE
template<class F,class S,class I,class T,int D> class FSMinput { 
#else

template<class F,class S,class I,class T,int D=0> class FSMinput { 
#endif
friend class fType;
    T index; // internal (index+1) for fast reference, 0 means unused
public:
    FSMinput(){index=0;}
};

// ------------------------------------------------------------------
// This is the main FSM class, which keeps all the internal data, and
// provides the user interface.
// The class keeps 3 dynamic pointer arrays with references to all existing
// states, inputs, and functions. The purpose: fast access via integer index
// There are also two 2D tables which control the operation of the FSM:
//   functTrans[] decides which function to fire for any combination
//                of input and state:                (input,state)->fun
//   stateTrans[] provides the next state in case there is no function,
//                or the function returns no state:  (input,state)->state
// Both 2D tables are referenced as 1D arrays, by converting the x,y
// combination to a single index - in the call to trIndex():
//      return (si-1)*numInputs +ii-1;
// ------------------------------------------------------------------
#ifdef PTL_CANNOT_INITIALIZE
template<class F,class S,class I,class T,int D> class FSM {
#else

template<class F,class S,class I,class T,int D=0> class FSM {
#endif
typedef S* (*FSMfun)(S*,void**); 
protected:
    S *curState;
    T numStates,numInputs,numFuncts;
    ArrayContainer<S*>    stateArr;
    ArrayContainer<I*>    inputArr;
    ArrayContainer<FSMfun> functArr;
    ArrayContainer<T> stateTrans;
    ArrayContainer<T> functTrans;
    // transform state/index index starting at 1 to table index starting at 0
    unsigned int trIndex(T s,T i){return (s-1)*(unsigned int)numInputs+i-1;}
    // the user adds states and inputs through addTrans()
    T addState(S *s);      // returns the index value, always >0
    T addInput(I *i);      // returns the index value, always >0
    T addFunct(FSMfun f);  // returns the index value, always >0
public:
    FSM():stateArr(8,1),inputArr(8,1),functArr(8,1),  // default sizes
              stateTrans(64,1),functTrans(64,1){
              numStates=0; numInputs=0; numFuncts=0; curState=NULL;
    }
    // start with estimates of the sizes, avoid re-allocation
    FSM(int ns,int ni,int nf):stateArr(ns,1),inputArr(ni,1),functArr(nf,1),
              stateTrans(ns*ni,1),functTrans(ns*ni,1){
              numStates=0; numInputs=0; numFuncts=0; curState=NULL;
    }
    ~FSM(){} // destructors for all ArrayCon's called automatically

    // functions to maintain the state machine
    void addTrans(I *i, S *s1, S *s2, FSMfun f);
    void remState(S *s); // remove state and all associated transitions
    void remInput(I *i); // remove input and all associated transitions
    void remTrans(I *i, S *s); // remove some or all transitions

    // functions to operate the state machine
    S *currentState(){return curState;}
    void setState(S *s){addState(s); curState=s;}
    S *fire(I *i);
};

// ------------------------------------------------------------------
// Fire the next stimulus input into the FSM. Fire the function
// specified for this combination of input/state, and get the new state.
// If the function does not return a new state, or if no function is given
// for this input/state combination, use the transition table to determine
// the next state. If no transition is given in the table (value=0),
// remain in the current state.
// ------------------------------------------------------------------
template<class F, class S, class I, class T, int D>
S*  fType::fire(I *i){
    unsigned int k; T si,fi,ik,sk; FSMfun f; S *s;

    if(!curState)PTLerror("cannot fire, no current state, ",curState);
    sk=((sType *)curState)->index; 
    ik=((iType *)i)->index; 
    if(sk>0 && ik>0){
        k=trIndex(sk,ik); // k=0 internal error that should never happen
        fi=functTrans[k]; 
        if(fi!=0)f=functArr[fi-1]; else f=NULL;
        if(f)s=(*f)(curState,NULL); else s=NULL;
        if(s==NULL){ si=stateTrans[k]; if(si!=0)s=stateArr[si-1]; }
        if(s)curState=s;
    }
    else PTLerror("firing unregistered state or input=",curState);
    return curState;
}

// ------------------------------------------------------------------
// Add a new state to the internal list of states. If this state is
// not recorded yet (index==0), then store the new index inside the state,
// and add the index to the list. If the state is already recorded, check
// that there aren't two states with the same index.
// ------------------------------------------------------------------
template<class F,class S,class I,class T,int D> 
T fType::addState(S *s){
    unsigned int n; T k;

    k=s->index;
    if(k==0){   // a new state, not recorded yet
        stateArr[numStates]=s;
        numStates++;
        s->index=numStates;
        if(numInputs>0){ // possibly increase the size of transition tables
            n=trIndex(numStates,numInputs);
            stateTrans[n]=0;
            functTrans[n]=0;
        }
    }
    else {   // this state is already used
        S* ss=stateArr[k-1];
        if(ss!=s)PTLerror("FSM, two states with the same index=",(void*)k);
    }
    return s->index;
}

// ------------------------------------------------------------------
// Add a new input to the internal list of inputs. If this input is
// not recorded yet (index==0), then store the new index inside the input,
// and add the index to the list. If the input is already recorded, check
// that there aren't two inputs with the same index.
// ------------------------------------------------------------------
template<class F,class S,class I,class T,int D> 
T fType::addInput(I *i){
    unsigned int k,m,n,ns,ni; T t;

    k=i->index;
    if(k==0){ // new input, not recorded yet
        inputArr[numInputs]=i;
        numInputs++;
        i->index=numInputs;
        // first increase the size of the transition table
        if(numStates>0){ // possibly increase the size of transition tables
            n=trIndex(numStates,numInputs);
            stateTrans[n]=0;
            functTrans[n]=0;
            m=numInputs;
            // re-format transition tables considering that we have more inputs
            for(; ;n--){ // proceed from the end of the array
                ns=n/m;    // new state index
                ni=n-ns*m; // new input index
                if(ni==m-1){stateTrans[n]=0; functTrans[n]=0;}
                else {
                    t=stateTrans[ns*(m-1)+ni]; stateTrans[n]=t;
                    t=functTrans[ns*(m-1)+ni]; functTrans[n]=t;
                }
                if(n==0)break;
            }
        }
    }
    else {   // this input is already used
        I* ii=inputArr[k-1];
        if(ii!=i)PTLerror("FSM, two states with the same index=",(void*)k);
    }
    return i->index;
}

// ------------------------------------------------------------------
// Add new function to the internal list. If the function is already
// recorded, there is nothing to be done. The function does not have
// to remember its index like state and input do.
// Returns: 0=some problem, >0 index of the function.
// ------------------------------------------------------------------
template<class F,class S,class I,class T,int D> 
T fType::addFunct(FSMfun f){
    T i;
    FSMfun p;

    for(i=0; i<numFuncts; i++){
        p=functArr[i];
        if(p==f)return i+1;
    }
    if(i>=numFuncts){    // only i==numFuncts can actually happen
        functArr[numFuncts]=f;
        numFuncts++;
    }
    return i+1;
}

// ------------------------------------------------------------------
// Record a new transition: When input i is applied, and s1 is
// the current state of the FSM, the FSM will invoke function f, and
// transfer either to the state returned by f or, if f returns NULL or no f
// is used for this transition, the FSM will transfer to s2.
// When i,s1,s2, or f are not registered prior to this call, they will be
// automatically registered.
// Any of the parameters can be NULL:
//   i=NULL, this transition applies to all registered inputs,
//   s1=NULL, this transition applies to any current state,
//   s2=NULL, the transition remains in its current state,
//   fun=NULL, no function used for this transition.
// Note that if f returns a new state, s2 is typically NULL, except
// when you want to use it as a default when f fails.
// f must be a member function of F, and should not access
// the private/protected members of FSM, such as curState.
// ------------------------------------------------------------------
template<class F,class S,class I,class T,int D> 
void fType::addTrans(I *i, S *s1, S *s2, FSMfun f){
    T ii,si,ti,fi, ii1,ii2,ss1,ss2,tt;
    int n;

    ii=0; if(i)ii=addInput(i);
    si=0; if(s1)si=addState(s1);
    ti=0; if(s2)ti=addState(s2);
    fi=0; if(f)fi=addFunct(f);

    if(ii==0){ii1=1; ii2=numInputs;} else {ii1=ii2=ii;}
    if(si==0){ss1=1; ss2=numStates;} else {ss1=ss2=si;}

    for(ii=ii1; ii<=ii2; ii++){
        for(si=ss1; si<=ss2; si++){
            if(!s2)ti=si; // remain in the same state, if no s2 given
            n=trIndex(si,ii);
            stateTrans[n]=ti;
            functTrans[n]=fi;
        }
    }
}
// ------------------------------------------------------------------
// Remove transition from the given input and state
//   i=NULL removes transitions for all inputs
//   s=NULL removes transitions for all states
// remTrans(NULL,NULL) wipes out all transitions
// ------------------------------------------------------------------
template<class F,class S,class I,class T,int D> 
void fType::remTrans(I *i, S *s){
    T ii,si, ii1,ii2,ss1,ss2;
    int n;

    ii=0; if(i)ii=addInput(i);   // if already recorded, nothing happens
    si=0; if(s)si=addState(s); // if already recorded, nothing happens

    if(ii==0){ii1=0; ii2=numInputs;} else {ii1=ii2=ii;}
    if(si==0){ss1=0; ss2=numStates;} else {ss1=ss2=si;}

    for(ii=ii1; ii<=ii2; ii++){
        for(si=ss1; si<=ss2; si++){
            n=trIndex(si,ii);
            stateTrans[n]=0;
            functTrans[n]=0;
        }
    }
}

// ------------------------------------------------------------------
// Remove the state and all associated transitions.
// Change the internal index of the the last state, and shift the last
// row of the transition table to the new position. Update the sizes
// of all tables.
// ------------------------------------------------------------------
template<class F,class S,class I,class T,int D> 
void fType::remState(S *s){
    S *ns; T ts,tf,k,j; int n;

    k=s->index;
    if(k==0)return;
    s->index=0;
    numStates--;
    ns=stateArr[numStates];
    ns->index=k;
    for(j=1;j<=numInputs;j++){
        n=trIndex(numStates,j);
        ts=stateTrans[n];
        tf=functTrans[n];
        n=trIndex(k,j);
        stateTrans[n]=ts;
        functTrans[n]=tf;
    }
}

// ------------------------------------------------------------------
// Remove the input and all associated transitions.
// Change the internal index of the the last input, and shift the last
// column of the transition table to the new position. Update the sizes
// of all tables.
// ------------------------------------------------------------------
template<class F,class S,class I,class T,int D> 
void fType::remInput(I *i){
    I *ni; T ts,tf,k,j; int m,n;

    k=i->index;
    if(k==0)return;
    i->index=0;
    numInputs--;
    ni=inputArr[numInputs];
    ni->index=k;
    for(j=0;j<=numStates;j++){
        n=trIndex(j,numInputs);
        ts=stateTrans[n];
        tf=functTrans[n];
        n=trIndex(j,k);
        stateTrans[n]=ts;
        functTrans[n]=tf;
    }
    // eliminate the last column from the transition tables
    m=n=0;
    for(k=0; k<numStates; k++){
        for(j=0; j<=numInputs; j++){  // numInputs is the new value already
            if(j<numInputs){
                ts=stateTrans[m]; stateTrans[n]=ts;
                tf=functTrans[m]; functTrans[n]=tf;
                n++;
            }
            m++;
        }
    }
}

// cancel all temporary renaming
#undef fType
#undef sType
#undef iType

#ifdef TEMPLATE_MANAGER
// --------------------------------------------------

// Include sections - needed only when the Template Manager is used
#define FSMInherit1(id,fsm,sta,inp,typ,ins) \
    public FSM<fsm,sta,inp,typ,ins>
#define FSMMember1(id,fsm,sta,inp,typ,ins)

#define FSMInherit2(id,fsm,sta,inp,typ,ins) \
    public FSMstate<fsm,sta,inp,typ,ins>
#define FSMMember2(id,fsm,sta,inp,typ,ins)

#define FSMInherit3(id,fsm,sta,inp,typ,ins) \
    public FSMinput<fsm,sta,inp,typ,ins>
#define FSMMember3(id,fsm,sta,inp,typ,ins)

// --------------------------------------------------
#endif  // TEMPLATE_MANAGER
#endif  // FSM_USED
#define FSM_USED
