// -------------------------------------------------------------
#include <iostream.h>
#define TEMPLATE_MANAGER
#include "pattern.h"
class Part;
class Plate;
class Bolt;
class Nut;
class Assembly;
class Product;
class Part : Pattern(Part) {
protected:
    int mPartNo;
public:
    Part(int n){mPartNo=n;}
    virtual void prt(int k){};
};
class Plate : public Part, Pattern(Plate) {
                         // inheritance of Part not part of composite
    int mx,my;
public:
    Plate(int n,int x,int y):Part(n){mx=x; my=y;}
    void prt(int k);
};
class Bolt : public Part, Pattern(Bolt) {
                         // inheritance of Part not part of composite
    int mDiam;
    Nut *mpNut; // knows its own nut
public:
    Bolt(int n,int d,Nut *np):Part(n){mDiam=d; mpNut=np;}
    void prt(int k);
};
class Nut : public Part { // does not need Pattern(..), is not in any pattern
    int mDiam;
public:
    Nut(int n,int d):Part(n){mDiam=d;}
    void prt(int k);
};
class Assembly : Pattern(Assembly) { // covers even inheritance from Part
public:
    Assembly(int n):Part(n){}
    void prt(int k);
};
class Product : Pattern(Product) {
    char *mpName; // product name
    Assembly *mpAssembly; // to the root assembly
public:
    Product(char *name,Assembly *root){mpName=name; mpAssembly=root;}
    void prt(); // print the composition of the entire product
};
void Plate::prt(int k){
    for(int i=0; i<k; i++) cout << "    ";
    cout << "Plate No." << mPartNo << " x=" << mx << " y=" << my << "\n";
}
void Nut::prt(int k){
    for(int i=0; i<k; i++) cout << "    ";
    cout << "Nut No." << mPartNo << " diam=" << mDiam << "\n";
}
void Bolt::prt(int k){
    for(int i=0; i<k; i++) cout << "    ";
    cout << "Bolt No." << mPartNo << " diam=" << mDiam << " -> "; mpNut->prt(0);
}
void Assembly::prt(int k){
    pattern_iterator_hier it;
    Part *pPart;
    for(int i=0; i<k; i++) cout << "    ";
    cout << "Assembly No." << mPartNo << "\n";
    ITERATE(it,this,pPart){
        pPart->prt(k+1);
    }
}
    
void Product::prt(){
    Plate *pPlate;
    // first print the assembly hierarchy
    cout << "Product=" << mpName << "\n";
    mpAssembly->prt(1);
    cout << "list of all plates:\n";
    pattern_iterator_plates it;
    ITERATE(it,this,pPlate) pPlate->prt(1);
}
    
int main(void){
    pattern Composite<Assembly,Part,0> hier; // hierarchy of Assemblies
    pattern Aggregate<Plate,Bolt,0> bolts; // 1-to-many relation, both ways
    pattern Collection<Product,Plate,0> plates; // all plates in the product
    Product *pProduct;
    Assembly *pRootAssembly;

    // using simplified names which permit table-like code in this case
    Assembly *a1,*a2; Plate *p; Bolt *b; Nut *n;

    // create a complex product from plates, bolts, and nuts
    pRootAssembly=new Assembly(1000);
    pProduct=new Product("myProduct",pRootAssembly);
    a1=new Assembly(1100); // assembly one
    p=new Plate(1110,34,22); plates.addTail(pProduct,p); hier.addTail(a1,p);
    n=new Nut(1101,10);                                  hier.addTail(a1,n);
    b=new Bolt(1102,10,n); bolts.addTail(p,b);           hier.addTail(a1,b);
    n=new Nut(1103,10);                                  hier.addTail(a1,n);
    b=new Bolt(1104,10,n); bolts.addTail(p,b);           hier.addTail(a1,b);
    a2=new Assembly(1100); // assembly two
    p=new Plate(1120,72,12); plates.addTail(pProduct,p); hier.addTail(a2,p);
    n=new Nut(1105,8);                                   hier.addTail(a2,n);
    b=new Bolt(1106,8,n); bolts.addTail(p,b);            hier.addTail(a2,b);
    p=new Plate(1130,14,14); plates.addTail(pProduct,p); hier.addTail(a2,p);
    n=new Nut(1107,6);                                   hier.addTail(a2,n);
    b=new Bolt(1108,6,n); bolts.addTail(p,b);            hier.addTail(a2,b);
    
    // nut & bolt for the root assembly, not linked to any plate
    n=new Nut(1501,8);    hier.addTail(pRootAssembly,n);
    b=new Bolt(1502,8,n); hier.addTail(pRootAssembly,b);
    hier.addTail(pRootAssembly,a1);
    hier.addTail(pRootAssembly,a2);
    
    pProduct->prt();
    return 0;
}
/* ------------------ results ------------------------
Product=myProduct
    Assembly No.1000
        Nut No.1501 diam=8
        Bolt No.1502 diam=8 -> Nut No.1501 diam=8
        Assembly No.1100
            Plate No.1110 x=34 y=22
            Nut No.1101 diam=10
            Bolt No.1102 diam=10 -> Nut No.1101 diam=10
            Nut No.1103 diam=10
            Bolt No.1104 diam=10 -> Nut No.1103 diam=10
        Assembly No.1100
            Plate No.1120 x=72 y=12
            Nut No.1105 diam=8
            Bolt No.1106 diam=8 -> Nut No.1105 diam=8
            Plate No.1130 x=14 y=14
            Nut No.1107 diam=6
            Bolt No.1108 diam=6 -> Nut No.1107 diam=6
list of all plates:
    Plate No.1110 x=34 y=22
    Plate No.1120 x=72 y=12
    Plate No.1130 x=14 y=14
--------------------------------------------------------*/
