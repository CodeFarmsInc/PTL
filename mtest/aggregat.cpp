#include <iostream>
using namespace std;
#define TEMPLATE_MANAGER
#include "pattern.h"

class A;
class B;

// only the SUN and WATCOM compilers require to comment out the end '{'
class A : Pattern(A) // {
    int mInt;
public:
    A(int i){mInt=i;}
    void prt(void){cout << mInt << "\n";}
};

// only the SUN and WATCOM compilers require to comment out the end '{'
class B : Pattern(B) // {
    int mInt;
public:
    B(int i){mInt=i;}
    void prt(void);
    static int cmp(const B* b1, const B* b2) { return (b1->mInt - b2->mInt);}
};

pattern Aggregate<A,B,1> aggr1;
pattern Aggregate<A,B,2> aggr2;

void B::prt(void){cout << "    " << mInt << "\n";
  A * pA;
  cout << "    par1="; 
  pA=((A*)(aggr1.parent(this))); if(pA) pA->prt(); else cout << "---null---\n";
  cout << "    par2="; 
  pA=((A*)(aggr2.parent(this))); if(pA) pA->prt(); else cout << "---null---\n";

}

int main(void){
    int i;
    pattern_iterator_aggr1 it1;
    pattern_iterator_aggr2 it2;
    A a(888); B *pB;
    
    for(i=0;i<5;i++){
        pB=new B(i); aggr1.addHead(&a,pB);
        pB=new B(i+10); aggr2.addTail(&a,pB);
    }
    aggr2.sort(&a,B::cmp);
    pB=new B(12); aggr2.addSorted(&a,pB,B::cmp);
    a.prt();
    ITERATE(it1,&a,pB){ pB->prt(); }
    a.prt();
    ITERATE(it2,&a,pB){ pB->prt(); }

    ITERATE(it1,&a,pB){ aggr1.remove(pB); delete pB; }
    ITERATE(it2,&a,pB){ aggr2.remove(pB); delete pB; }
    return(0);
}
