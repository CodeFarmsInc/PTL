// ----------------------------------------------------------------------
// This test is similar to arraycon.cpp, except that both arrays
// are attached to class C.
// ----------------------------------------------------------------------
#include <iostream>
using namespace std;
#include <array.h>

class A {
    char mChar;
public:
    A(){mChar=' ';}
    A(char c){mChar=c;}
    void set(char c){mChar=c;}
    void prt(){cout << "A=" << mChar << "\n";}
};
class B {
    int mInt;
public:
    B(){mInt=0;}
    B(int i){mInt=i;}
    void prt(){cout << "B=" << mInt << "\n";}
    static int cmp(const void *v1,const void *v2){ // compare for sort
                         return ((B*)v1)->mInt - ((B*)v2)->mInt;}
};

// some compilers (e.g. SUN) cannot initialize template parameters
#ifdef PTL_CANNOT_INITIALIZE
// holder of the two arrays
class C : public ArrayHolder<C,A,0>, public ArrayHolder<C,B,0> {
public:
    void C::prt(char whichArray, char *label);
};

static Array<C,A,0> aArray;
static Array<C,B,0> bArray;

#else
// compilers with full handling of templates
// holder of the two arrays
class C : public ArrayHolder<C,A>, public ArrayHolder<C,B> {
public:
    void C::prt(char whichArray, char *label);
};

static Array<C,A> aArray;
static Array<C,B> bArray;

#endif

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
