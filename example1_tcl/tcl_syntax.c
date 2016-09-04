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
  memset(syntax->chars, 0, syntax->charsNum);
}

void tcl_syntax_uninit(struct tcl_syntax *syntax) {
  free(syntax->nodes);
  free(syntax->chars);
}

void tcl_syntax_push(struct tcl_syntax *syntax,
                     unsigned int depth,
                     unsigned int pos,
                     enum tcl_syntax_node_type type,
                     const char *str,
                     unsigned int strLen) {



  struct tcl_syntax_node *cur=NULL;

  //try to reuse an existing node
  if(syntax->nodesNext != 0) {
    struct tcl_syntax_node *top;
    top=&syntax->nodes[syntax->nodesNext-1];

    if(type==tcl_syntax_sep && top->depth==depth && top->type==tcl_syntax_sep) {
      cur=top;
    } else if(type==tcl_syntax_sep && top->depth==depth && top->type==tcl_syntax_spc) {
      top->type=tcl_syntax_sep;
      cur=top;
    } else if(type==tcl_syntax_str && top->depth==depth && top->type==tcl_syntax_str) {
      cur=top;
    }
  }

  //get new node
  if(!cur) {
    //allocate node
    if(syntax->nodesNext >= syntax->nodesNum) {
      (syntax->nodesNum)*=2;
      size_t size=sizeof(struct tcl_syntax_node)*syntax->nodesNum;
      syntax->nodes=(struct tcl_syntax_node*)realloc(syntax->nodes,size);
    }

    //init node
    cur=&(syntax->nodes[syntax->nodesNext]);
    cur->type=type;
    cur->depth=depth;
    cur->pos=pos;
    cur->charsNum=0;
    syntax->nodesNext++;
  }

  //
  if(strLen > 0) {
    // printf("'%.*s'\n",strLen,str);
    unsigned int initFrom=syntax->charsNum;

    //
    if(syntax->charsNext+strLen+1 >= syntax->charsNum) {
      //calc new chars max num
      while(syntax->charsNext+strLen+1 >= syntax->charsNum) {
        syntax->charsNum*=2;
      }

      //realloc syntax chars and init
      syntax->chars=(char*)realloc(syntax->chars,syntax->charsNum);
      memset(&syntax->chars[initFrom],0,syntax->charsNum-initFrom);
    }

    //set syntax chars next before null terminator from prev
    if(cur->charsNum>0) {
      syntax->charsNext--;
    }

    //set str
    memcpy(&syntax->chars[syntax->charsNext],str,strLen);
    cur->charsNum+=strLen;
    syntax->charsNext+=strLen+1;
  }
}
