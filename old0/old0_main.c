#include "old0_tcl_parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

char *string_from_file(const char *fn) {
  FILE *file;
  unsigned int dataSize;
  char *str;

  if(!(file = fopen(fn, "rb"))) {
    return 0;
  }

  fseek(file,0L,SEEK_END);
  dataSize = ftell(file);
  fseek(file,0L,SEEK_SET);
  str=(char*)malloc(dataSize+1);
  str[dataSize]='\0';
  fread(str,1,dataSize,file);
  fclose(file);

  return str;
}

int main() {
  char *txt=string_from_file("test.tcl");

  bool err;
  struct parmac stk[2048],*p;
  struct tcl_parser tp;

  p=stk;
  tp.errMsg=NULL;
  tp.markStart=txt;
  tp.markEnd=txt;
  tp.pos=0;
  tp.row=0;
  tp.col=0;

  tcl_parser_main_machine(p,txt);

  while(parmac_run(&p,&tp,&err)) {
  }

  printf("\n");

  if(err) {
    printf("Error.\n");

    if(tp.errMsg!=NULL) {
      printf(tp.errMsg);
    }
  }

    printf("done %p.\n",p);
  return 0;

}
