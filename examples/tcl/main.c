#include "tcl_parser.h"

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

  struct tcl_syntax_stmt *stmts=NULL;

  struct parmac stk[2048];
  struct parmac *p=stk;
  tcl_parser_main_machine(p,txt);

  bool err;
  char errMsg[2048];

  while(p=parmac_run(p,NULL,&err,errMsg,sizeof(errMsg),true)) {

  }

  // printf("done '%s\n",);
  return 0;

}
