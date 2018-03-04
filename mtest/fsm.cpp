#include <iostream>
using namespace std;
#define TEMPLATE_MANAGER
#include "pattern.h"

class MyFSM;
class MyState;
class MyInput;

// only the SUN and WATCOM compilers require to comment out the end '{'
class MyState : Pattern(MyState) // {
public:
    virtual void prt(){}
};

// only the SUN and WATCOM compilers require to comment out the end '{'
class MyInput : Pattern(MyInput) // {
    char mChar;
public:
    MyInput(char c){mChar=c;}
    virtual void prt(){cout<<" input="<<mChar;}
};

class MyState1 : public MyState {
    int mInt;
public:
    MyState1(int i){mInt=i;}
    void prt(){cout<<" state="<<mInt;}
};

class MyState2 : public MyState {
    char mChar;
public:
    MyState2(char c){mChar=c;}
    void prt(){cout<<" state="<<mChar;}
};

// only the SUN and WATCOM compilers require to comment out the end '{'
class MyFSM : Pattern(MyFSM) // {
public:
    void runSequence(MyInput *inp1,MyInput *inp2);

    // transition functions
    static MyState* fun1(MyState *s,void **argv){cout<<" fun1"; return s;}
    static MyState* fun2(MyState *s,void **argv){cout<<" fun2"; return NULL;}
};

void MyFSM::runSequence(MyInput *inp1,MyInput *inp2){
   MyState *pMyState;
   cout<<"starting "; currentState()->prt(); cout<<"\n";
   inp2->prt(); pMyState=fire(inp2); pMyState->prt(); cout<<"\n";
   inp1->prt(); pMyState=fire(inp1); pMyState->prt(); cout<<"\n";
   inp1->prt(); pMyState=fire(inp1); pMyState->prt(); cout<<"\n";
   inp1->prt(); pMyState=fire(inp1); pMyState->prt(); cout<<"\n";
   inp2->prt(); pMyState=fire(inp2); pMyState->prt(); cout<<"\n";
}

// following line only needed for the Template Manager, foo is never used
pattern FSM<MyFSM,MyState,MyInput,char,0> foo;

int main(){
   MyState *pMyState1,*pMyState2,*pMyState3;
   MyInput *pMyInput1,*pMyInput2;
   MyFSM fsm;  

   // build the FSM
   pMyInput1=new MyInput('1'); 
   pMyInput2=new MyInput('2'); 
   
   pMyState1=new MyState2('A'); 
   pMyState2=new MyState1(7); 
   pMyState3=new MyState2('B'); 
  
   fsm.setState(pMyState1); // start from this state
   fsm.addTrans(pMyInput1,pMyState1,pMyState3,MyFSM::fun1);
   fsm.addTrans(pMyInput2,pMyState1,pMyState2,NULL);
   fsm.addTrans(pMyInput1,pMyState2,pMyState3,MyFSM::fun2);
   fsm.addTrans(pMyInput2,pMyState2,pMyState1,NULL);
   fsm.addTrans(NULL,pMyState3,pMyState1,MyFSM::fun2); // for any input

   fsm.runSequence(pMyInput1,pMyInput2);
   fsm.remState(pMyState3);
   fsm.runSequence(pMyInput1,pMyInput2);

   return 0;
}
