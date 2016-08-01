#include "tcl_parser_util.h"
#include <stdio.h>

void tcl_parser_util_countRowCol(const char *src,const char*srcTo,int *row,int *col,
                 int *rowStart,int *rowLen) {
  const char *it=src;

  *row=0;
  *col=0;
  *rowStart=0;
  *rowLen=0;

  while(it!=srcTo) {
    if(it[0]=='\n') {
      (*row)++;
      (*col)=0;
      (*rowStart)=(int)(it-src)+1;
    } else if(it[0]!='\r') {
      (*col)++;
    }

    it++;
  }

  *rowLen=*col;

  while(it[0]!='\0' && it[0]!='\n') {
    if(it[0]!='\r') {
      (*rowLen)++;
    }

    it++;
  }
}

void tcl_parser_util_printSyntaxError(const char *src,const char *srcPos,const char *errMsg) {
  int row,col,rowStart,rowLen;
  tcl_parser_util_countRowCol(src,srcPos,&row,&col,&rowStart,&rowLen);

  //
  printf("\n");

  //
  printf("'%.*s'\n",rowLen,&src[rowStart]);

   int i;

  for(i=0;i<col+1;i++) {
    printf(" ");
  }

  printf("^\n");

  //
  printf("Error: %s at pos %i, line %i, col %i.\n",
         errMsg?errMsg:"Unexpected syntax",
         (int)(srcPos-src),row+1,col+1);

}
