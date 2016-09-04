#include "tcl_parser.h"
#include "tcl_expr_parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>




void tcl_expr_parser_run(struct tcl_parser *tp,
                    struct tcl_syntax *syntax,
                    const char *src) {
  PARMAC_DEPTH stkDepth=0;

  tp->depth=0;
  tp->errMsg=NULL;
  tp->src=src;
  tp->syntax=syntax;
  tp->sqrbCount=0;

  tcl_expr_parser_main_machine(tp->stk,0);

  while(parmac_run(tp->stk,&stkDepth,tp)) {
    if(stkDepth+1==tp->stkNum) {
      tp->stkNum*=2;
      tp->stk=(struct parmac*)realloc(tp->stk,sizeof(struct parmac)*tp->stkNum);
    }
  }

  if(parmac_failed(tp->stk) || (tp->src[parmac_last_pos(tp->stk,stkDepth)]!='\0')) {
    printf("Error.\n");

    if(tp->errMsg!=NULL) {
      printf(tp->errMsg);
    }

    tcl_parser_util_printSyntaxError(src,&tp->src[parmac_last_pos(tp->stk,stkDepth)],tp->errMsg);

  } else {

  }
}

void tcl_parser_run(struct tcl_parser *tp,
                    struct tcl_syntax *syntax,
                    const char *src) {
  PARMAC_DEPTH stkDepth=0;

  tp->depth=0;
  tp->errMsg=NULL;
  tp->src=src;
  tp->syntax=syntax;
  tp->sqrbCount=0;

  tcl_parser_main_machine(tp->stk,0);

  while(parmac_run(tp->stk,&stkDepth,tp)) {
    if(stkDepth+1==tp->stkNum) {
      tp->stkNum*=2;
      tp->stk=(struct parmac*)realloc(tp->stk,sizeof(struct parmac)*tp->stkNum);
    }
  }

  if(parmac_failed(tp->stk) || (tp->src[parmac_last_pos(tp->stk,stkDepth)]!='\0')) {
    printf("Error.\n");

    if(tp->errMsg!=NULL) {
      printf(tp->errMsg);
    }

    tcl_parser_util_printSyntaxError(src,&tp->src[parmac_last_pos(tp->stk,stkDepth)],tp->errMsg);

  } else {

  }


  //


  unsigned int i,c=0;

  for(i=0;i<tp->syntax->nodesNext;i++) {
    struct tcl_syntax_node *cur=&syntax->nodes[i];
    // print_spaces(cur->depth*2);
    printf("%u : %u : ",cur->depth,cur->pos);

    if(cur->type==tcl_syntax_str) {
      if(cur->charsNum==0) {
        printf("str ''");
      } else {
        printf("str '%s'",&tp->syntax->chars[c]);
        c+=cur->charsNum+1;
      }
    } else if(cur->type==tcl_syntax_spc) {
      printf("spc");
    } else if(cur->type==tcl_syntax_sep) {
      printf("sep");
    }

    printf("\n");
  }

  printf("\n");
}


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

void print_spaces(unsigned int s) {
  while(s>0) {
    printf(" ");
    s--;
  }
}

int main() {
  char *txt=string_from_file("test6.tcl");

  if(!txt) {
    return 1;
  }

  struct tcl_syntax syntax;
  struct tcl_parser tp;

  tcl_syntax_init(&syntax);

  tp.stkNum=2;
  tp.stk=(struct parmac*)malloc(sizeof(struct parmac)*tp.stkNum);


  tcl_parser_run(&tp,&syntax,txt);
  printf("\n\n");
  tcl_expr_parser_run(&tp,&syntax,"1+-(+2*7)");


  free(tp.stk);
  tcl_syntax_uninit(&syntax);
#ifdef _MSC_VER
  system("pause");
#endif

  return 0;
}
