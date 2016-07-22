#ifndef TCL_SYNTAX_H
#define TCL_SYNTAX_H

#ifndef __cplusplus
#include <stdbool.h>
#endif

#include <string.h>
#include <stdlib.h>

enum tcl_syntax_node_type {
  tcl_syntax_str,
  tcl_syntax_spc,
  tcl_syntax_sep,
};

struct tcl_syntax_node {
  unsigned int depth,pos,row,col;
  enum tcl_syntax_node_type type;
  unsigned int charsNum;
};

struct tcl_syntax {
  unsigned int nodesNum,nodesNext,charsNum,charsNext;
  struct tcl_syntax_node *nodes;
  char *chars;
};

#ifdef __cplusplus
extern "C" {
#endif

  void tcl_syntax_init(struct tcl_syntax *syntax);
  void tcl_syntax_uninit(struct tcl_syntax *syntax);

  struct tcl_syntax_node *tcl_syntax_push(struct tcl_syntax *syntax,
                                     unsigned int depth,
                                     unsigned int pos,
                                     enum tcl_syntax_node_type type);


  char *tcl_syntax_str_push(struct tcl_syntax *syntax,unsigned int len);

#ifdef __cplusplus
}
#endif

#endif
