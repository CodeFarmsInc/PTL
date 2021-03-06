// ----------------------------------------------------------------------
// This test is similar to arraycon.cpp, except that both arrays
// are attached to class C.
// ----------------------------------------------------------------------
#include <iostream>
using namespace std;
#define TEMPLATE_MANAGER
#include "pattern.h"

// only the SUN and WATCOM compilers require to comment out the end '{'
class A  Pattern(A) // {
    char mChar;
public:
    A(){mChar=' ';}
    A(char c){mChar=c;}
    void set(char c){mChar=c;}
    void prt(){cout << "A=" << mChar << "\n";}
};

// only the SUN and WATCOM compilers require to comment out the end '{'
class B  Pattern(B) // {
    int mInt;
public:
    B(){mInt=0;}
    B(int i){mInt=i;}
    void prt(){cout << "B=" << mInt << "\n";}
    static int cmp(const void *v1,const void *v2){ // compare for sort
                         return ((B*)v1)->mInt - ((B*)v2)->mInt;}
};

// holder of the two arrays
// only the SUN and WATCOM compilers require to comment out the end '{'
class C : Pattern(C) // {
public:
    void C::prt(char whichArray, char *label);
};

pattern Array<C,A,0> aArray;
pattern Array<C,B,0> bArray;

void C::prt(char whichArray,char *label){
    if(whichArray=='a')cout << label << 
        " size=" << aArray.size(this) << " used=" << aArray.used(this) <<"\n";
    else if(whichArray=='b')cout << label << 
        " size=" << bArray.size(this) << " used=" << bArray.used(this) <<"\n";
    else cout << label << " wrong first parameter in C::prt()\n"; 
}

int main(void){
    unsigned i;
    unsigned char q;
    A* pA;
    B* pB;
    C* pC=new C; // single holder all through this text

    bArray.initial(3,0);

    // create some initial array with arbitrarily initialized A objects
    for(i=0, q='p'; i<8; i++, q++){
        pA=new A(q); aArray.array(pC)[i]=(*pA);
    }
    pC->prt('a',"first array");
    // push in two new objects, then pop one
    pA=new A('f'); aArray.push(pC,pA);
    pA=new A('g'); aArray.push(pC,pA);
    pC->prt('a',"after push ");
    aArray.pop(pC)->prt();
    pC->prt('a',"after pop ");

    // test different ways of changing the size of the array
    aArray.reduce(pC);
    pC->prt('a',"after reduce");
    aArray.cut(pC,4);
    pC->prt('a',"after cut   ");

    // print the array just to see it is still as it should be
    for(i=0;i<aArray.used(pC);i++){ 
        cout << "i=" << i << " "; aArray.array(pC)[i].prt();
    }

    // experiment with the second array
    for(i=4; i<=10; i=i+2){
        pB=new B(i); bArray.array(pC)[i]=(*pB);
    }
    pC->prt('b',"array bArray,");
    bArray.grow(pC,100);
    pC->prt('b',"after grow");
    pB=new B(22); bArray.insert(pC,2,pB);
    bArray.remove(pC,3);

    // print the array
    for(i=0; i<bArray.used(pC); i++){
        cout << "i=" << i << " "; bArray.array(pC)[i].prt();
    }

    // sort the array  and print it
    bArray.sort(pC,B::cmp);
    cout << "\nafter sorting\n";
    for(i=0; i<bArray.used(pC); i++){
        cout << "i=" << i << " "; bArray.array(pC)[i].prt();
    }
    
    return(0);
}
