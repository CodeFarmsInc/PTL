#include <iostream>
using namespace std;
#include <aggregat.h>
#include <mgr.h>

class A;
class B;
class A : public AggregateParent<A,B,1>, public AggregateParent<A,B,2> { 
    int mInt;
public:
    A(int i){mInt=i;}
    void prt(void){cout << mInt << "\n";}
};
class B : public AggregateChild<A,B,1>, public AggregateChild<A,B,2> { 
    int mInt;
public:
    B(int i){mInt=i;}
    void prt(void);
    static int cmp(const B* b1, const B* b2) { return (b1->mInt - b2->mInt);}
};

static Aggregate<A,B,1> aggr1;
static Aggregate<A,B,2> aggr2;

void B::prt(void){cout << "    " << mInt << "\n";
  A * pA;
  cout << "    par1="; 
  pA=((A*)(aggr1.parent(this))); if(pA) pA->prt(); else cout << "---null---\n";
  cout << "    par2="; 
  pA=((A*)(aggr2.parent(this))); if(pA) pA->prt(); else cout << "---null---\n";

}

int main(void){
    int i;
    AggregateIterator<A,B,1> it1;
    AggregateIterator<A,B,2> it2;
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
