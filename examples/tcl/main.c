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

void printSyntax(struct tcl_syntax_stmt *stmt,int depth) {
  struct tcl_syntax_word *word;
  struct tcl_syntax_sub *sub;
  int i;

  for(;stmt;stmt=stmt->next) {
    for(i=0;i<depth;i++) {printf("  ");}
    printf("stmt:\n");

    for(word=stmt->words;word;word=word->next) {
    for(i=0;i<depth+1;i++) {printf("  ");}
      printf("  word:\n");

      for(sub=word->subs;sub;sub=sub->next) {
        for(i=0;i<depth+2;i++) {printf("  ");}
        if(sub->type==tcl_syntax_sub_str) {
          printf("   str: '%s'\n",sub->valStr);
        } else if(sub->type==tcl_syntax_sub_var) {
          printf("   var: '%s'\n",sub->valStr);
        } else if(sub->type==tcl_syntax_sub_stmt) {
          printf("   cmd:\n");
          printSyntax(sub->valStmt,depth+1);
        }
      }
    }
  }
}

int main() {
  char *txt=string_from_file("test.tcl");

  if(!txt) {
    return 1;
  }

  struct tcl_parser tp;

  tcl_parser_init(&tp);
  tcl_parser_run(&tp,txt);
  tcl_parser_uninit(&tp);

#ifdef _MSC_VER
  system("pause");
#endif
  return 0;
}
