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

struct tcl_syntax_block {

};

struct tcl_syntax_node {
  unsigned int depth,pos,row,col;
  enum tcl_syntax_node_type type;
  unsigned int charsNum;
};

struct tcl_syntax {
  unsigned int blocksNum,blocksNext,nodesNum,nodesNext,charsNum,charsNext;
  struct tcl_syntax_block *blocks;
  struct tcl_syntax_node *nodes;
  char *chars;
};

#ifdef __cplusplus
extern "C" {
#endif

  void tcl_syntax_init(struct tcl_syntax *syntax);
  void tcl_syntax_uninit(struct tcl_syntax *syntax);

  // void tcl_syntax_push_block(struct tcl_syntax *syntax);
  void tcl_syntax_push(struct tcl_syntax *syntax,
                       unsigned int depth,
                       unsigned int pos,
                       enum tcl_syntax_node_type type,
                       const char *str,
                       unsigned int strLen);

#ifdef __cplusplus
}
#endif

#endif
