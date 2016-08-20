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
  char *txt=string_from_file("test6.tcl");

  if(!txt) {
    return 1;
  }

  struct tcl_syntax syntax;
  struct tcl_parser tp;

  tcl_syntax_init(&syntax);


  tcl_parser_init(&tp);
  tcl_parser_run(&tp,&syntax,txt);


  unsigned int i,c=0;

  for(i=0;i<syntax.nodesNext;i++) {
    struct tcl_syntax_node *cur=&syntax.nodes[i];
    printf("%u : %u : ",cur->depth,cur->pos);

    if(cur->type==tcl_syntax_str) {
      printf("str '%s'",&syntax.chars[c]);
      c+=cur->charsNum+1;
    } else if(cur->type==tcl_syntax_spc) {
      printf("spc");
    } else if(cur->type==tcl_syntax_sep) {
      printf("sep");
    }

    printf("\n");
  }

  printf("\n");


  tcl_parser_uninit(&tp);
  tcl_syntax_uninit(&syntax);
#ifdef _MSC_VER
  system("pause");
#endif

  return 0;
}
