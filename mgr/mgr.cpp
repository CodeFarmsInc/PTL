// ***********************************************************************
//                       Code Farms Inc.
//          7214 Jock Trail, Richmond, Ontario, Canada, K0A 2Z0
// 
//                     Copyright (C) 2002
//        This software is subject to copyright protection under
//        the laws of Canada, United States, and other countries.
// ************************************************************************

// Template Manager for the Pattern Template Library
// Algorithm:
// (1) Run through all the input skipping commented sections,
//     until you find tokens "pattern" or "Pattern".
// (2) Analyze syntax: pattern name<type1,type2,..> inst;
//                     Pattern(inst);
//     and record it in an internal data structure.
// (3) Create file pattern.h using references to *.g files
//     with the path determined from the mgr call itself.
//     File names are identical to pattern names, but all in small case
//     and reduced to 8 characters.
// (4) If any Pattern() statements are detected, run crosscheck
//     between the 'pattern' and 'Pattern' statements, and print
//     a warning message if any discrepancy found. 
//     
// Syntax:
//    gen source.cc <pattern.g>
// ----------------------------------------------------------------------

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define DOS

class PatternGenerator;
class Pattern {
friend class PatternGenerator;
    static char BLANK;
    static int TOKEN_LIMIT;
    char **tokens;    // array of tokens
    Pattern *next;   // list of patterns
    int n;           // number of tokens
public:
    Pattern();
    ~Pattern();
    void addToken(char *s,int sz);
    char *lastToken(int *i){char *p=NULL; *i=0;
          if(n>0){p=tokens[n-1]; *i=strlen(p);} return(p);}
    void genParam(FILE *fo);
};
int Pattern::TOKEN_LIMIT=10;
char Pattern::BLANK=' ';

void Pattern::genParam(FILE *fo){
    int k;    // index to traverse parameters

    fprintf(fo,"(%s",tokens[n-1]);
    for(k=1; k<n-1; k++){
        fprintf(fo,",%s",tokens[k]);
    }
    fprintf(fo,")");
}

Pattern::~Pattern(){
    int i;
    for(i=0;i<n;i++)delete tokens[i];
    delete tokens;
}

// For a given string and its size (generally not NULL ending)
// add a token to the list
void Pattern::addToken(char *s,int sz){
    char *p;
    int i;
        
    if(n>=TOKEN_LIMIT){
        printf("gen() internal error: %d tokens exceeded\n",TOKEN_LIMIT);
        return;
    }
    p=new char[sz+1];
    if(!p){printf("gen() internal allocation failure\n"); return;}
    // copy all non-blank characters
    tokens[n]=p;
    for(i=0;i<sz;i++){
        if(s[i]!=BLANK){ *p=s[i]; p++;}
    }
    *p='\0';
    n++;
}
Pattern::Pattern(){ n=0; next=NULL;
    tokens=new char* [TOKEN_LIMIT];
    if(!tokens)printf("internal allocation problem\n");
}

class ClassRecord {
friend class PatternGenerator;
    static char BLANK;
    ClassRecord *next;
    char *name;
public:
    ClassRecord(char *n,int sz,ClassRecord *nxt);
};
char ClassRecord::BLANK=' ';

ClassRecord::ClassRecord(char *n,int sz,ClassRecord *nxt){ 
    int i; char *p;
    next=nxt; name=new char[sz+1];
    if(!name)printf("internal error in allocating a name\n");
    else { // skip blanks when copying
        for(p=name, i=0;i<sz;i++){
            if(n[i]!=BLANK){ *p=n[i]; p++;}
        }
        *p='\0';
    }
}

class PatternGenerator {
    static int BUFFER_SIZE;
    static char LINE;
    static char BLANK;
    static char* pattern_token;
    static char* participate_token;
    Pattern *patList;
    ClassRecord *classList;    // list of classes in pattern declarations
    ClassRecord *registerList; // list of classes registered for patterns
    FILE *fi,*fo; // input, output files
    char *path;
    char *buff;
    char *last;
    char *fillBuffer(void);
    char *nextToken(char *p,int *sz,int nest);
    char* fillPattern(char *s);
    char* moveTo(char *p,char *t);
    char* nextPattern(char *s);
    void syntaxError(int i);
    void addClass(char *n,int sz);
    void registerClass(char *n,int sz);
    void genFile(void);
    void crossCheck(void);
    void genIncludes(void);
    void genIterators(void);
    void fixName(char *b,char *n);
    void debugToken(char *t,int sz);
    void duplicates(void);
    int cmpST(char *s,char *t,int n){ // s zero ending, t given by length
        int i=n-strlen(s); if(i==0)i=strncmp(s,t,n); return(i);}
    int numberToken(char *t,int sz); // return 1 if all numbers
public:
    PatternGenerator(char *inp,char *out,char *libPath);
    ~PatternGenerator();
    void generate(void);
};
int PatternGenerator::BUFFER_SIZE=256;
char PatternGenerator::LINE='\n';
char PatternGenerator::BLANK=' ';
char* PatternGenerator::pattern_token="pattern ";
char* PatternGenerator::participate_token="Pattern";

// --------------------------------------------------------------
// Generate sections that inject pointers into application classes
// Overloaded new() is always the same, pre-sets all pointers to NULL
// Friend statements must be provided by the *.g file.
// --------------------------------------------------------------
void PatternGenerator::genFile(void){
    ClassRecord *cr;
    Pattern *pp;
    int i; // index for traversing tokens of one pattern
    char *c1; // control of printing commas

    // generate inheritance for classes involved in any pattern
    for(cr=classList; cr; cr=cr->next){
        fprintf(fo,"\n#define pattern_%s",cr->name);
      
        // first go through all the patterns
        for(pp=patList, c1=" "; pp; pp=pp->next){       
            // check whether this class is used 
            for(i=1; i<pp->n-1; i++){ // 0=pattern, n-1=its instance
                if(!strcmp(cr->name,pp->tokens[i])){
                    fprintf(fo,"%s\\\n  %sInherit%d",c1,pp->tokens[0],i);
                    c1=",";
                    pp->genParam(fo);
                }
            }
        }
        fprintf(fo," { ");

        // next go through all parts that have to be injected
        for(pp=patList; pp; pp=pp->next){       
            // check whether this class is used 
            for(i=1; i<pp->n-1; i++){
                if(!strcmp(cr->name,pp->tokens[i])){
                    fprintf(fo,"\\\n  %sMember%d",pp->tokens[0],i);
                    pp->genParam(fo);
                }
            }
        }
        fprintf(fo," PTL_COMMENT \n");
    }
    fprintf(fo,"\n");
}

void PatternGenerator::genIterators(void){
    Pattern *pp;
    int k;    // index to traverse parameters

    // first go through all the patterns
    for(pp=patList; pp; pp=pp->next){       
        fprintf(fo,"\n#define pattern_iterator_%s",pp->tokens[pp->n-1]);
        fprintf(fo," \\\n   %sIterator",pp->tokens[0]);
        fprintf(fo,"<%s",pp->tokens[1]);
        for(k=2; k<pp->n-1; k++){
            fprintf(fo,",%s",pp->tokens[k]);
        }
        fprintf(fo,">");
    }
    fprintf(fo,"\n");
}

// --------------------------------------------------------------
// Cross-check that all classes used in the pattern declarations
// have the appropriate registration Pattern(className).
// When some registrations are missing, print a warning message.
// Implementation note:
// The speed of this function can be improved by sorting the two
// lists, and then comparing them linearly.
// --------------------------------------------------------------
void PatternGenerator::crossCheck(void){
    ClassRecord *c,*r; // for traversing classList and registerList

    // each-to-each simple check one way
    for(c=classList; c; c=c->next){
        for(r=registerList; r; r=r->next){
            if(!strcmp(c->name,r->name))break;
        }
        if(!r && strcmp(c->name,"char") && strcmp(c->name,"int")
              && strcmp(c->name,"float") && strcmp(c->name,"double")){
            printf("mgr warning: Pattern(%s) seems to be missing\n",c->name);
        }
    }
    // each-to-each simple check the other way
    for(r=registerList; r; r=r->next){
        for(c=classList; c; c=c->next){
            if(!strcmp(c->name,r->name))break;
        }
        if(!c)printf("mgr warning: Pattern(%s) but class not in any pattern\n",
                                                                r->name);
    }
}

// --------------------------------------------------------
// Copy and customize all *.g files needed for this program
// --------------------------------------------------------
void PatternGenerator::genIncludes(void){
    Pattern *pp;         // walking through all patterns
    char fileName[80];   // buffer to assemble the file name
    
    for(pp=patList; pp; pp=pp->next){
        fixName(fileName,pp->tokens[0]);
        fprintf(fo,"#include <%s%s.h>\n",path,fileName);
    }
}

// --------------------------------------------------------------------
// Convert name 'n' to lower case, max 8 characters, and copy it into b.
// (This is all hard coded, because function tolower() changes lower case
//  characters if present)
// --------------------------------------------------------------------
void PatternGenerator::fixName(char *b,char *n){
    int i;
    unsigned int a,A,Z; // letters a,A,Z
    unsigned c;         // character to be converted

    a='a'; A='A'; Z='Z';
    for(i=0; i<8; i++){
        c=n[i];
        if(c>=A && c<=Z)c=a+c-A;
        b[i]=c;
        if(n[i]=='\0')break;
    }
    b[i]='\0';
}
    
// --------------------------------------------------
// Do not accept the class name if already on the list
// n=name of the class to add, sz=its length (size).
// This function is called when a class is mentioned
// in a pattern declaration.
// --------------------------------------------------
void PatternGenerator::addClass(char *n,int sz){
    ClassRecord *cr;  // pointer just to traverse

    for(cr=classList; cr; cr=cr->next){
        if(!(this->cmpST(cr->name,n,sz)))return;
    }
    cr=new ClassRecord(n,sz,classList); classList=cr;
}
    
// --------------------------------------------------
// Do not accept the class name if already on the list
// n=name of the class to add, sz=its length (size).
// This function is called if a class is registered
// as participating in a patern.
// --------------------------------------------------
void PatternGenerator::registerClass(char *n,int sz){
    ClassRecord *cr;  // pointer just to traverse

    for(cr=registerList; cr; cr=cr->next){
        if(!(this->cmpST(cr->name,n,sz))){
            printf("error: two declarations of Pattern(%s)\n",cr->name);
            return;
        }
    }
    cr=new ClassRecord(n,sz,registerList); registerList=cr;
}

void PatternGenerator::syntaxError(int i){
    char *p;
    printf("MGR SYNTAX ERROR No.%d\n",i);
    for(p=buff; *p!=LINE; p++)printf("%c",*p);
    printf("\n");
}

// The string that specifies the library path is externally allocated
PatternGenerator::PatternGenerator(char *inp,char *out,char *libPath){
    patList=NULL;
    classList=NULL;
    registerList=NULL;
    path=libPath;
    fi=fopen(inp,"r");
    fo=fopen(out,"w");
    if(!fi || !fo){
        printf("error: cannot open one of these files: %s,%s\n",inp,out);
        return;
    }
    buff=new char[BUFFER_SIZE];
    if(!buff){printf("allocation error in gen()\n"); return;}
}
    
PatternGenerator::~PatternGenerator(){
    Pattern *p,*s;
    ClassRecord *cr,*cn;

    for(p=patList; p; p=s){
        s=p->next;
        delete p;
    }
    for(cr=classList; cr; cr=cn){
        cn=cr->next;
        if(cr->name)delete cr->name;
        delete cr;
    }
    if(fi)fclose(fi); 
    if(fo)fclose(fo); 
    delete buff;
}

// Eliminate duplicates of patterns where all parameters match
void PatternGenerator::duplicates(void){
    Pattern *p1,*p2,*lastP;
    int i,ii;

    // Compare each pattern to all others
    for(p1=patList; p1; p1=p1->next){       
        for(lastP=p1, p2=p1->next; p2; ){       
            // compare pattern and parameters, 0=pattern, n-1=its instance
            i=p2->n-1; ii=p1->n-1;
            if(i==ii){
                for(i=0; i<ii; i++){ // 0=pattern, n-1=its instance
                    if(strcmp(p1->tokens[i],p2->tokens[i]))break;
                }
            }
            if(i==ii){ // eliminate entry p2, it duplicates p1
                lastP->next=p2->next;
                delete p2;
                p2=lastP->next;
            }
            else {  // just move to the next p2 on the list
                lastP=p2;
                p2=p2->next;
            }
        }
    }
}

// This is the main code generation algorithm
void PatternGenerator::generate(void){
    char *s;
    
    for(s=fillBuffer(); s;){
        s=nextPattern(s); // returns "pattern" or "Pattern" or NULL
        if(s)s=fillPattern(s);
    }
    fclose(fi); fi=NULL;
    duplicates();
    genIncludes();
    genFile(); 
    genIterators(); 
    fclose(fo); fo=NULL;
    if(registerList)crossCheck();
}

// -------------------------------------------------
// debugging printout of one token
// t=token, not null ending
// sz=length (size) in bytes
// -------------------------------------------------
void PatternGenerator::debugToken(char *t, int sz){
    int i; // index for the loop

    printf("debug token: t=%s sz=%d token=",t,sz);
    for(i=0;i<sz;i++)printf("%c",t[i]);
    printf("\n");
}

// ---------------------------------------------------------------
// Return 1 if this token is composed only of digits
// ---------------------------------------------------------------
int PatternGenerator::numberToken(char *t,int sz){
    int i; 
    for(i=0;i<sz;i++)if(t[i]<'0' || t[i]>'9')return(0);
    return(1);
}

// ---------------------------------------------------------------
// record "pattern" or "Pattern"
// ---------------------------------------------------------------
char* PatternGenerator::fillPattern(char *s){
    Pattern *p;  // new pattern on which to store the information
    char *t,*tr;  // general tokens
    int sz,sr;    // sizes of t,tr
    int s1,s2;
    
    // sizes are needed, because tokens are not 0 ending
    s1=strlen(pattern_token);
    s2=strlen(participate_token);

    if(!strncmp(pattern_token,s,s1)){
        t=moveTo(s," "); *t=','; // introduce a special token
        t=nextToken(t,&sz,0);
        p=new Pattern; p->next=patList; patList=p;
        p->addToken(t,sz);
        t=nextToken(t,&sz,0); if(*t!='<'){syntaxError(1); return(t);}
        for(;;){
            t=nextToken(t,&sz,1); p->addToken(t,sz); 
            t=nextToken(t,&sz,1); 
            tr=p->lastToken(&sr);
            if(*t=='>' && this->numberToken(tr,sr))break;
            addClass(tr,sr); // break skips the last parameter (int)
            if(*t=='>')break;
            if(*t!=','){syntaxError(2); return(t);}
        }
        t=nextToken(t,&sz,0); p->addToken(t,sz);
        t=nextToken(t,&sz,0);
        if(*t!=';' && *t!='('){syntaxError(3); return(t);}
    }
    
    else if(!strncmp(s,participate_token,s2)){
        t=nextToken(s,&sz,0); 
        if(*t!='('){syntaxError(4); return(t);}
        t=nextToken(t,&sz,0); 
        registerClass(t,sz);
        t=nextToken(t,&sz,0); 
        if(*t!=')'){syntaxError(5); return(t);}
        t=nextToken(t,&sz,0); 
        if(*t!='{' && sz!=0){syntaxError(6); return(t);}
    }
    else printf("internal error in detecting 'pattern' or 'Pattern'\n");
    return(t);
}

// Move from character p until you find one of the characters in t.
// Reload buffer if needed, return NULL if never found
char* PatternGenerator::moveTo(char *p,char *t){
    char *s,*u;
    for(s=p;;s++){
        if(*s==LINE){s=fillBuffer(); if(!s)break;}
        for(u=t;*u!='\0';u++)if(*u==(*s))return(s);
    }
    return(NULL);
}

// Search for string "pattern" or "Pattern"
char* PatternGenerator::nextPattern(char *s){
    char *p;
    int s1,s2;
    
    // sizes are needed, because tokens are not 0 ending
    s1=strlen(pattern_token);
    s2=strlen(participate_token);

    for(p=s; p; p++ ){
        if(*p==LINE)p=fillBuffer();
        if(!p)return(NULL);
        if(!strncmp(pattern_token,p,s1))return(p);
        if(!strncmp(participate_token,p,s2))return(p);
    }
    return(NULL);
}

// -----------------------------------------------------------
// Fill the next line from the input file. At the same time, 
// eliminate the text which is commented out, including nested
// C-type comments that may stretch over several lines.
// -----------------------------------------------------------
char* PatternGenerator::fillBuffer(void){
    int i,k,lev; // i,k=indexes, lev=level of nesting
    char *p;     // a pointer to walk through the text

    while(fgets(buff,BUFFER_SIZE,fi)){
        // eliminate comments from the buffer
        for(p=buff; *p!='\n'; p++){
            if(*p=='/' && *(p+1)=='/'){*p='\n'; break;}
        }
        if(p==buff)continue; // entire line wiped out

        // C-type comments can stretch over several lines
        for(i=k=0, lev=0; ; i++){
            if(lev>0){
                if(buff[i]=='*' && buff[i+1]=='/'){lev--; if(lev==0)i++;}
            }
            else {
                if(buff[i]=='/' && buff[i+1]=='*'){lev++; i=i+2;}
                else {buff[k]=buff[i]; k++;}
            }
            if(buff[i]==LINE){
                if(k==0){
                    i=0;
                    p=fgets(buff,BUFFER_SIZE,fi); 
                    if(!p)return(NULL);
                }
                else return(buff);
            }
        }
        
    }
    return(NULL);
}

// If last token is a special character, next must be a true token.
// If last token is a true token, next must be a special character.
// Pass blanks as part of true tokens.
// When nest=1, include anything within <> brackets.
char* PatternGenerator::nextToken(char *p, int *sz, int nest){
    char *t,*s;
    int i,k,st,special,lev;

    t="<>;,()"; st=strlen(t); // special tokens
    // was the last token a special token?
    for(i=0; i<st; i++)if(*p==t[i])break;
    if(i<st)special=1; else special=0;

    // search for the next special token
    for(s=p+1, k=lev=0; ; k++){
        if(s[k]==LINE){if(special)break; s=fillBuffer(); if(!s)return(NULL);}
        for(i=0; i<st; i++) if(s[k]==t[i])break;
        if(i<st){
            if(nest){
                if(s[k]=='<')lev++;
                else if(s[k]=='>' && lev>0)lev--;
                else if(lev==0)break;
            }
            else break;
        }
    }
    while(*s==' '){s++; k--;}  // skip leading blanks
    if(special){*sz=k; return(s);}
    else       {*sz=1; return(&(s[k]));}
}

    
int main(int argc,char **argv){
    int n;
    char *p1,*p2,*p3;
    PatternGenerator *pg;

    if(argc<2 || argc>3){
        printf("syntax: gen input <pattern.h>\n");
        return(1);
    }
    p1=argv[1];
    if(argc<3)p2="pattern.h";  else p2=argv[2];
    // get the path by stripping the last 3 characters from the call
    // and changing 'gen' to 'lib'
    for(p3=argv[0], n=strlen(argv[0])-1; n>0; n--){
        if(p3[n]=='\\' || p3[n]=='/')break;
    }
    if(n>=8){
        p3=new char[n+9]; strncpy(p3,argv[0],n+1); p3[n+1]='\0';
        strcat(p3,"../lib/");
        p3[n+7]=p3[n+3]=p3[n]; // make this DOS/UNIX independent
    }
    else p3="";
    pg=new PatternGenerator(p1,p2,p3);
    pg->generate();
    delete pg;
    return(0);
}
