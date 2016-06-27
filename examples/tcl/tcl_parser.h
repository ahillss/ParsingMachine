#ifndef TCL_PARSER_H
#define TCL_PARSER_H

#ifndef __cplusplus
#include <stdbool.h>
#endif

#include <string.h>
#include <stdlib.h>

// #define TCL_DEBUG_PARSE_FSM 1

#define TCL_USE_VARS
#define TCL_USE_CMDS
#define TCL_USE_QSTRS
#define TCL_USE_BSTRS
#define TCL_USE_SSTRS

#include "../../parmac.h"

#define endof(x) (x+sizeof(x)/sizeof(*x))

struct tcl_syntax_stmt;

enum tcl_syntax_sub_type {
  tcl_syntax_sub_str,
  tcl_syntax_sub_var,
  tcl_syntax_sub_stmt
};

struct tcl_syntax_sub {
  enum tcl_syntax_sub_type type;
  char *valStr;
  struct tcl_syntax_stmt *valStmt;
  struct tcl_syntax_sub *next;
};

struct tcl_syntax_word {
  struct tcl_syntax_sub *subs;
  struct tcl_syntax_word *next;
};

struct tcl_syntax_stmt {
  struct tcl_syntax_word *words;
  struct tcl_syntax_stmt *next;
};

struct tcl_syntax_build {
  struct tcl_syntax_stmt *firstStmt, *lastStmt;
  struct tcl_syntax_word *firstWord, *lastWord;
  struct tcl_syntax_sub *firstSub, *lastSub;
  struct tcl_syntax_build *next;
};

struct tcl_parser {
	unsigned int closingsInd;
  struct parmac *stk;
  unsigned int stkNum;
  const char *errMsg;
  const char *markStart,*markEnd;
  int pos,row,col;
  char *closings;

  struct tcl_syntax_build *buildStk;
  struct tcl_syntax_stmt *rootStmt;
  int recurseDepth;
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
