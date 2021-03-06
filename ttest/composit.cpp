using namespace std;
#include <iostream>
#include <mgr.h>
#include <collecti.h>
#include <composit.h>

// -------------------------------------------------------------
// The following test is a typical application of pattern Composite,
// combined with another pattern (here Collection). 
// The Root keeps an additional collection of Graphic objects.
// -------------------------------------------------------------

class Picture;
class Graphic;
class Root;

class Root : public CollectionParent<Root,Graphic,0> {
public:
    ~Root();
};

class Graphic :
 public CollectionChild<Root,Graphic,0>,
 public CompositeChild<Picture,Graphic,1> {
public:
    virtual void prt(int i){}
    virtual ~Graphic(){}
    virtual int isComposite(Collection<Picture,Graphic,1> *c){c=c; return(0);}
    virtual Graphic *searchFun(void *v){return NULL;}
};

class Picture : public Graphic, public CompositeParent<Picture,Graphic,1> {
    int mID;
public:
    Picture(int id){mID=id;}
    virtual void prt(int i);
    virtual int isComposite(Collection<Picture,Graphic,1> *c){c=c; return(1);}
    virtual Graphic *searchFun(void *v){return NULL;}
};

struct Position { int x,y;};

class Line : public Graphic {
    int mX1,mY1,mX2,mY2;
public:
    Line(int x1,int y1,int x2,int y2){mX1=x1; mY1=y1; mX2=x2; mY2=y2;}
    virtual void prt(int i){cout <<
          "   Line:" << mX1 <<"," << mY1 << " " << mX2 << "," << mY2; }
    // search for line starting at the given position
    virtual Graphic *searchFun(void *v){ Position* pPosition=(Position*)v;
        Line* pLine=NULL; if(mX1==pPosition->x && mY1==pPosition->y)pLine=this;
        return (Graphic*)pLine;}
};

Graphic *search(Graphic *g,void *v){return g->searchFun(v);}

class Text : public Graphic {
    char *mpTxt;
public:
    Text(char *t){mpTxt=t;} // not keeping its own copy
    virtual void prt(int i){cout << "   Text:" << mpTxt;}
    virtual Graphic *searchFun(void *v){return NULL;}
};

static Composite<Picture,Graphic,1> comp;
static Collection<Root,Graphic,0>   coll;

Root::~Root(){
    CollectionIterator<Root,Graphic,0> itColl;
    Graphic *pGraphic;

    ITERATE(itColl,this,pGraphic){
        coll.remove(this,pGraphic);
        delete pGraphic;
    }
}

void Picture::prt(int i){  // for i=1 recursive print of the subtree
    CompositeIterator<Picture,Graphic,1> itComp;
    Graphic *pGraphic;
    
    cout << "   Picture:"<< mID;
    if(i>0){
        cout << "\nPicture:" << mID << "\n";
        ITERATE(itComp,this,pGraphic) pGraphic->prt(1);
    }
}

int main(void){
    CollectionIterator<Root,Graphic,0> itColl;
    CompositeIterator<Picture,Graphic,1> itComp;
    Picture *pPicture1,*pPicture2,*pPict3;
    Graphic *pGraphic;
    Root *pRoot; 
    Line *pLine;
    Text *pText;
    Position position;
    int k;
    
    pRoot=new Root;
    pPicture1=new Picture(111);
    pPicture2=new Picture(222);
    pPict3=new Picture(333);

    comp.addHead(pPicture1,pPicture2);
    comp.addHead(pPicture2,pPict3); 
    coll.addHead(pRoot,pPicture1); 
    coll.addHead(pRoot,pPict3);   // note pPicture2 not on the collection

    pLine=new Line(1,1,11,11);
    coll.addHead(pRoot,pLine); comp.addHead(pPicture1,pLine);
    pLine=new Line(2,2,22,22);
    coll.addHead(pRoot,pLine); // note: only on collection
    pLine=new Line(3,3,33,33);
    coll.addHead(pRoot,pLine); comp.addHead(pPict3,pLine);

    pText=new Text("TEXT1");
    coll.addHead(pRoot,pText); comp.addHead(pPicture1,pText);
    pText=new Text("TEXT2");
    coll.addHead(pRoot,pText); comp.addHead(pPicture2,pText);
    pText=new Text("TEXT3");
    coll.addHead(pRoot,pText); comp.addHead(pPict3,pText);
    
    cout << "\n\nComposite:\n"; pPicture1->prt(1);
    cout << "\n\nCollection:\n";
    k=1;
    ITERATE(itColl,pRoot,pGraphic){
        pGraphic->prt(0);
        if(k>=4){k=0; cout << "\n";}; k++; // formatting printout
    }

    position.x=3; position.y=3; // search for line starting at (3,3)
    pLine=(Line*)(comp.depthFirst(pPicture1,search,&position));
    cout << "\nSearch for line starting at (3,3):\n";
    if(pLine) pLine->prt(0); else cout << "NOT ANY"; cout << "\n";

    position.x=2; position.y=2; // search for line starting at (2,2)
    pLine=(Line*)(comp.depthFirst(pPicture1,search,&position));
    cout << "\nSearch for line starting at (2,2):\n";
    if(pLine) pLine->prt(0); else cout << "NOT ANY"; cout << "\n";
    
    comp.dissolve(pPicture1);
    delete pRoot;
    return(0);
}
