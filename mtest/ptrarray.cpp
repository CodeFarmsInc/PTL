#include <iostream>
using namespace std;
#define TEMPLATE_MANAGER
#include "pattern.h"

// only the SUN and WATCOM compilers require to comment out the end '{'
class A Pattern(A) // {
    char mChar;
public:
    A(char i){mChar=i;}
    void prt(){cout << " A=" << mChar << "\n";}
};

// only the SUN and WATCOM compilers require to comment out the end '{'
class B Pattern(B) // {
    int mInt;
public:
    B(int i){mInt=i;}
    void prt(){cout << " B=" << mInt << "\n";}
    static int cmp(const void *v1,const void *v2); // for the sort
};
int B::cmp(const void *v1,const void *v2){
    B** ppB1=(B**)v1;
    B** ppB2=(B**)v2;
    if(*ppB1==NULL)return -1;
    if(*ppB2==NULL)return 1;
    return (*ppB1)->mInt - (*ppB2)->mInt;
}

// only the SUN and WATCOM compilers require to comment out the end '{'
class C : Pattern(C) // {
};

pattern PtrArray<C,A,0> aArr;     // character driven array of A*
pattern PtrArray<C,B,0> bArr;     // int driven small array of B*

int main(void){

    long i,size;
    unsigned char c;
    A *a;
    B *b;

    bArr.initial(6); // all bArr arrays startSz=6 on all C objects from now on
    C* h=new C; // holder for both arrays

    for(i=4, c='p'; i<8; i++, c++){
        a=new A(c);
        aArr.set(h,c,a);
    }
    cout<<"size="<<aArr.size(h)<<" used="<<aArr.used(h)<<
                                     " count="<<aArr.count(h)<<"\n";
    size=aArr.size(h);
    for(i=0;i<size;i++){ 
        a=aArr.get(h,i);
        if(a){cout<<"i="<<i; a->prt();}
    }

    for(i=4; i<8; i++){
        b=new B(i); bArr.set(h,i,b);
    }
    cout<<"array bArr, size="<<bArr.size(h) << " used=" << bArr.used(h) << "\n";
    bArr.grow(h,100);
    cout<<" after grow size="<<bArr.size(h) << " used=" << bArr.used(h) << "\n";
    // test access through operator [], remove() and insert()
    cout << "bArr[4] access: "; bArr.array(h)[4]->prt();
    b=new B(22); bArr.insert(h,2,b);
    bArr.remove(h,3);
    size=bArr.size(h); // use 'size', not 'used' to test initialization
    for(i=0; i<size; i++){
        b=bArr.get(h,i);
        if(b){cout<<"i="<<i; b->prt();}
    }
    cout<<"count="<<bArr.count(h)<<"\n";
    cout<<"sorted array:\n";
    bArr.sort(h,B::cmp); // sort by value of b in the pointed objects
    for(i=0; i<size; i++){
        b=bArr.get(h,i);
        if(b){cout<<"i="<<i; b->prt();}
    }
    cout << "before clean size=" << bArr.size(h) << " used=" << bArr.used(h)
         <<" count="<<bArr.count(h)<<"\n";
    bArr.clean(h);
    cout << "after clean size=" << bArr.size(h) << " used=" << bArr.used(h)
         <<" count="<<bArr.count(h)<<"\n";
    return(0);
}
