#include <iostream>
using namespace std;
#include <collecti.h>

class A;
class B;
class A : public CollectionParent<A,B,0>, public CollectionParent<A,B,1> { 
    int mCode;
public:
    A(int ii):CollectionParent<A,B,0>(),CollectionParent<A,B,1>(){mCode=ii;}
    void prt(void){cout << mCode << '\n';}
};
class B : public CollectionChild<A,B,0>, public CollectionChild<A,B,1> { 
    int mVal;
public:
    B(int ii):CollectionChild<A,B,0>(),CollectionChild<A,B,1>(){mVal=ii;}
    void prt(void){cout << "    " << mVal << '\n';}
    static int cmp(const B* b1, const B* b2) { return (b1->mVal - b2->mVal);}
};

int main(void){
try {
    int n;
    long k;
    Collection<A,B,0> col0;
    Collection<A,B,1> col1;
    CollectionIterator<A,B,0> it0;
    CollectionIterator<A,B,1> it1;

    // short variable names permit table-like code style when creating the data
    A a(888); B *b, *bp[5];
    
    for(n=0;n<5;n++){
        b=bp[n]=new B(n);
        col0.addHead(&a,b);
        col1.addTail(&a,b);
    }
    col0.sort(&a,B::cmp);
    // try some of the add..() functions, also insert() and append()
    b=new B(-1); col0.addSorted(&a,b,B::cmp); col1.insert(&a,bp[0],b);
    b=new B(3);  col0.addSorted(&a,b,B::cmp); col1.insert(&a,bp[3],b);
    b=new B(5);  col0.addSorted(&a,b,B::cmp); col1.append(&a,bp[3],b);
    b=new B(-2); col0.addHead(&a,b);          col1.append(&a,bp[4],b);
    b=new B(6);  col0.addTail(&a,b);          col1.insert(&a,bp[4],b);

    a.prt();
    ITERATE(it0,&a,b){ b->prt(); }
    a.prt();
    RETRACE(it0,&a,b){ b->prt(); }
    a.prt();
    ITERATE(it1,&a,b){ b->prt(); }

    ITERATE(it0,&a,b){
        col0.remove(&a,b);
        col1.remove(&a,b);
        delete b;
    }
}
catch(PTLerrorHandler){
    cout << "caught PTL error\n";
    cout.flush();
}
    return(0);
}
