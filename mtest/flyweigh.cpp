// ***********************************************************************
//                       Code Farms Inc.
//           P.O. Box 1270, Richmond, Ontario, Canada, K0A 2Z0
// 
//                   Copyright (C) 1994, 1995
//        This software is subject to copyright protection under
//        the laws of Canada, United States, and other countries.
// ************************************************************************

// --------------------------------------------------------------------
// Practical test of the Flyweight pattern, where the extrinsic
// object is a Line of text, with characters represented by 1 byte codes.
// It is assumed that the text can switch between two fonts, Roman
// and Italics. When printing the line, two Flyweight instances are
// used, one for each font.
// Note how Line calculates the position of each Character by adding
// the width of the preceeding characters. Special characters are 
// used for switching between the two text modes:
//    '{'=italic mode, '}'=normal mode
// In a real system, these would be some of the unprintable characters,
// but here, for the demonstration purpose, we chose characters that
// we can easily see and print.
//
// This program only emulates the printing by printing out the position
// for each letter. The height of characters is assumed to be the same
// for the entire Line. The end of line is marked by '\n' character.
//
// Author: Jiri Soukup, Mar.1/95, major upgrade Feb.18/97
// ----------------------------------------------------------------------
#include <iostream>
using namespace std;
#define TEMPLATE_MANAGER
#include "pattern.h"

class LineState;
class Line;
class CharacterState;
class Character;

// only the SUN and WATCOM compilers require to comment out the end '{'
class LineState : Pattern(LineState) // {
friend class Line;
    int mX,mY,mSize;
public:
    void getLineState(int *x,int *y,int *sz){*x=mX; *y=mY; *sz=mSize;}
};

// only the SUN and WATCOM compilers require to comment out the end '{'
class Line : Pattern(Line) // {
friend class Flyweight<Line,Character,LineState,CharacterState>;
    static char ITALICS;
    static char NORMAL;
public:
    // Line starting at X,Y with vertSize of characters
    Line(int x, int y, int vertSize){ 
            LineState* pLineState=getFlyweightState();
            pLineState->mX=x; pLineState->mY=y; pLineState->mSize=vertSize;}
    virtual ~Line(){}
    void draw(char *text,
            Flyweight<Line,Character,LineState,CharacterState>* roman,
            Flyweight<Line,Character,LineState,CharacterState>* italic);
};

char Line::ITALICS='{';
char Line::NORMAL='}';

// only the SUN and WATCOM compilers require to comment out the end '{'
class CharacterState : Pattern(CharacterState) // {
friend class Character;
    int mX,mY1,mY2; // x is the width, y1,y2 vertical range
    static int mDefaultSize; 
public:
    void getCharState(int *x,int *y){*x=mX; *y=mDefaultSize;}
    void setCharState(int x,int y1,int y2){
        mX=x; mY1=y1; mY2=y2; if(mDefaultSize<mY2)mDefaultSize=mY2; }
};

int CharacterState::mDefaultSize=0;

// This class may store lot of data and use sophisticated functions,
// here is just a watered down version to test the concept.
// only the SUN compiler requires to comment the end '{'
class Character : Pattern(Character) // {
friend class Flyweight<Line,Character,LineState,CharacterState>;
    char mByte;  // one character representation
    char mFont;  // 'n'=normal, 'i'=italic; ??? not needed ?
public:
    int draw(LineState *sp); // invoked only by the Flyweight pattern
    Character(char c,char font,int x,int y1,int y2){
                   this->setCharState(x,y1,y2); mFont=font; mByte=c;}
};

// ------------------------------------------
// print one line of externally provided text
// ------------------------------------------
void Line::draw(char *text,
    Flyweight<Line,Character,LineState,CharacterState>* roman,
    Flyweight<Line,Character,LineState,CharacterState>* italic){

    char *pChar;
    int xs,ys;
    CharacterState *pCharacterState;
    Flyweight<Line,Character,LineState,CharacterState>* pFlyweight=roman;
    LineState *pLineState=getFlyweightState();

    for(pChar=text; *pChar!='\n'; pChar++){
        if(*pChar==ITALICS)pFlyweight=italic;
        else if(*pChar==NORMAL)pFlyweight=roman;
        else {
            pFlyweight->fun(*pChar,&Character::draw,this);
            // update the position after drawing this character
            pCharacterState=pFlyweight->getState(*pChar);
            pCharacterState->getCharState(&xs,&ys); //scale=pLineState->mSize/ys
            pLineState->mX = pLineState->mX + (xs*(pLineState->mSize))/ys;
        }
    }
}

// -------------------------------------------------------------------
// Use the information about the extrinsic state to draw the character
// in the correct position and in the appropriate scale.
// It may return >0 when error, but that is not used here.
// -------------------------------------------------------------------
int Character::draw(LineState *pLineState){
    int rx1,rx2,ry1,ry2,ss,yy;

    CharacterState *pCharacterState=getFlyweightState();
    pLineState->getLineState(&rx1,&yy,&ss);
    float scale=ss/(float)(CharacterState::mDefaultSize);
    rx2=rx1 + (pCharacterState->mX) * scale;
    ry1=yy + (pCharacterState->mY1) * scale;
    ry2=yy + (pCharacterState->mY2) * scale;
    cout<<mByte<<"("<<mFont<<"):("<<rx1<<","<<ry1<<" "<<rx2<<","<<ry2<<")\n";
    return(0);
}

pattern Flyweight<Line,Character,LineState,CharacterState> italic;
pattern Flyweight<Line,Character,LineState,CharacterState> roman;

int main(void){
   // load some characters into the italic and roman flyweights
   Character *pChar;

   pChar=new Character('i','n',30,0,100);  roman.add(pChar,'i');
   pChar=new Character('t','n',40,0,100);  roman.add(pChar,'t');
   pChar=new Character('a','n',60,0,100);  roman.add(pChar,'a');
   pChar=new Character('g','n',60,-30,70); roman.add(pChar,'g');
   pChar=new Character('e','n',70,0,70);   roman.add(pChar,'e');
   pChar=new Character('r','n',70,0,70);   roman.add(pChar,'r');
   pChar=new Character(' ','n',100,0,0);   roman.add(pChar,' ');

   pChar=new Character('w','i',80,0,70);   italic.add(pChar,'w');
   pChar=new Character('a','i',70,0,70);   italic.add(pChar,'a');
   pChar=new Character('s','i',70,0,70);   italic.add(pChar,'s');
   pChar=new Character(' ','n',100,0,0);   italic.add(pChar,' ');

   // create a line with a given scale
   Line line(1000,8000,50);

   //  print a line of text in this position
    line.draw("it {was} a tiger\n",&roman,&italic);
    return(0);
}
