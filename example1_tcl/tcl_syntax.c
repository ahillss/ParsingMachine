#include "tcl_syntax.h"

#include <stdio.h>
#include <stddef.h>


void tcl_syntax_init(struct tcl_syntax *syntax) {
  syntax->nodesNum=2;
  syntax->nodesNext=0;
  syntax->nodes=(struct tcl_syntax_node*)malloc(sizeof(struct tcl_syntax_node)*syntax->nodesNum);

  syntax->charsNum=16;
  syntax->charsNext=0;
  syntax->chars=(char*)malloc(syntax->charsNum);
}

void tcl_syntax_uninit(struct tcl_syntax *syntax) {
  free(syntax->nodes);
  free(syntax->chars);
}

struct tcl_syntax_node *tcl_syntax_push(struct tcl_syntax *syntax,
                                        unsigned int depth,
                                        unsigned int pos,
                                        enum tcl_syntax_node_type type //,
                                        // unsigned int strMaxLen
                                        ) {

  //
  struct tcl_syntax_node *top;
  top=&syntax->nodes[syntax->nodesNext-1];

  if(type==tcl_syntax_sep && top->depth==depth && top->type==tcl_syntax_sep) {
    return top;
  }

  if(type==tcl_syntax_sep && top->depth==depth && top->type==tcl_syntax_spc) {
    top->type=tcl_syntax_sep;
    return top;
  }

  // if(type==tcl_syntax_str && top->depth==depth && top->type==tcl_syntax_str) {
  //   return top;
  // }

  //
  if(syntax->nodesNext >= syntax->nodesNum) {
    (syntax->nodesNum)*=2;
    syntax->nodes=(struct tcl_syntax_node*)realloc(syntax->nodes,sizeof(struct tcl_syntax_node)*syntax->nodesNum);
  }

  //
  struct tcl_syntax_node *cur=&(syntax->nodes[syntax->nodesNext]);
  cur->type=type;
  cur->depth=depth;
  cur->pos=pos;
  cur->charsNum=0;
  syntax->nodesNext++;

  return cur;
}

char *tcl_syntax_str_push(struct tcl_syntax *syntax,unsigned int len) {
  if(syntax->charsNext+len >= syntax->charsNum) {
    while(syntax->charsNext+len >= syntax->charsNum) {
      syntax->charsNum*=2;
    }

    syntax->chars=(char*)realloc(syntax->chars,syntax->charsNum);
  }

  char *r=&syntax->chars[syntax->charsNext];
  syntax->charsNext+=len;
  return r;
}
