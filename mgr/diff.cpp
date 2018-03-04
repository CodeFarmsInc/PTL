// ***********************************************************************
//                       Code Farms Inc.
//          7214 Jock Trail, Richmond, Ontario, Canada, K0A 2Z0
// 
//                     Copyright (C) 1996
//        This software is subject to copyright protection under
//        the laws of Canada, United States, and other countries.
// ************************************************************************

// Customized diff - reports the first line on which the files disagree.
// Syntax: diff file1 file2
// Author: J.Soukup, Oct.8, 1989
// --------------------------------------------------------------

#include <stdio.h>
#define BSIZE 128
int main(int argc,char **argv) {
    int i,k;
    char buf1[BSIZE],buf2[BSIZE],*f1,*f2;
    FILE *file1,*file2;

    if(argc<2){printf("diff: wrong syntax\n"); return(1);}
    file1=fopen(argv[1],"r");
    file2=fopen(argv[2],"r");
    if(!file1){printf("diff: cannot open file=%s\n",argv[1]); return(1);}
    if(!file2){printf("diff: cannot open file=%s\n",argv[2]); return(1);}
    
    for(i=0; ;){
        f1=fgets(buf1,BSIZE,file1);
        f2=fgets(buf2,BSIZE,file2);
        if(!f1 && !f2)return(0); /* normal exit */
        if(!f1){
            printf("diff: %s has %d lines, %s is longer\n",argv[1],i,argv[2]);
            return(1);
        }
        if(!f2){
            printf("diff: %s has %d lines, %s is longer\n",argv[2],i,argv[1]);
            return(1);
        }
        i++;
        for(k=0; buf1[k]!='\n'; k++){
            if(buf1[k]!=buf2[k]){
                printf("%s %s: first different line=%d\n",argv[1],argv[2],i);
                return(1);
            }
        }
        if(buf2[k]!='\n'){
            printf("%s %s: first different line=%d\n",argv[1],argv[2],i);
            return(1);
        }
    }
}
