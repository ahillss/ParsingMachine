#ifndef TCL_PARSER_H
#define TCL_PARSER_H

#ifndef __cplusplus
#include <stdbool.h>
#endif

#include <string.h>
#include <stdlib.h>

#include "../../parmac.h"

#define endof(x) (x+sizeof(x)/sizeof(*x))

enum tcl_syntax_type {
  tcl_syntax_str,
  tcl_syntax_spc,
  tcl_syntax_sep,
};

struct tcl_syntax {
  unsigned int depth,pos,row,col;
  enum tcl_syntax_type type;
  unsigned int charsNum;
};

struct tcl_parser {
  const char *src;
  unsigned int closingsInd;
  struct parmac *stk;
  unsigned int stkNum;
  const char *errMsg;
  int pos,row,col;
  char *closings;

  unsigned int depth;
  unsigned int syntaxNum,syntaxNext;
  struct tcl_syntax *syntax;
  unsigned int syntaxCharsNum,syntaxCharsNext;
  char *syntaxChars;
};

#ifdef __cplusplus
extern "C" {
#endif

  void tcl_parser_init(struct tcl_parser *tp);
  void tcl_parser_uninit(struct tcl_parser *tp);
  void tcl_parser_run(struct tcl_parser *tp,const char *src);

#ifdef __cplusplus
}
#endif

#endif
