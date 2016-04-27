#ifndef TCL_PARSER_H
#define TCL_PARSER_H

#ifndef __cplusplus
#include <stdbool.h>
#endif

// #define TCL_DEBUG_PARSE_FSM 1

#define TCL_USE_VARS
#define TCL_USE_CMDS
#define TCL_USE_QSTRS
#define TCL_USE_BSTRS
#define TCL_USE_SSTRS

#include "../../parmac.h"


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


struct tcl_parser_syntax_builder {
  struct tcl_syntax_stmt *rootStmt, *lastStmt;
  struct tcl_syntax_word *rootWord, *lastWord;
  struct tcl_syntax_sub *rootSub, *lastSub;
  const char *markStart,*markEnd;
};

// struct tcl_parser_syntax_builder *builder

#ifdef __cplusplus
extern "C" {
#endif

  void tcl_parser_main_machine(struct parmac *p,const char *src);
  void tcl_parser_init(struct parmac **p, const char *src);

#ifdef __cplusplus
}
#endif

#endif
