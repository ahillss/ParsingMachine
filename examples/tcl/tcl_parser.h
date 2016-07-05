#ifndef TCL_PARSER_H
#define TCL_PARSER_H

#ifndef __cplusplus
#include <stdbool.h>
#endif

#include <string.h>
#include <stdlib.h>

#include "../../parmac.h"

#define endof(x) (x+sizeof(x)/sizeof(*x))
// struct tcl_syntax_block {
//   unsigned int stmtsInd,stmtsNum;
// };

// struct tcl_syntax_stmt {
//   unsigned int parentInd,wordsInd,wordsNum;
// };

// struct tcl_syntax_word {
//   unsigned int parentInd,subsInd,subsNum;
// };

// struct tcl_syntax_sub {
//   unsigned int parentInd,valInd;
//   enum tcl_syntax_sub_type type;
// };

// struct tcl_syntax_block;

enum tcl_syntax_sub_type {
  tcl_syntax_sub_str,
  tcl_syntax_sub_var,
  tcl_syntax_sub_cmd
};

struct tcl_syntax_sub {
  enum tcl_syntax_sub_type type;
  union {
    char *str;
    struct tcl_syntax_block *block;
  } data;
};

struct tcl_syntax_word {
  struct tcl_syntax_sub *subs;
  unsigned int subsNum;
};

struct tcl_syntax_stmt {
  struct tcl_syntax *words;
  unsigned int wordsNum;
};

struct tcl_syntax_block {
  struct tcl_syntax_stmt *stmts;
  unsigned int stmtsNum;
};



struct tcl_parser {
  unsigned int closingsInd;
  struct parmac *stk;
  unsigned int stkNum;
  const char *errMsg;
  const char *markStart,*markEnd;
  int pos,row,col;
  char *closings;

  int recurseDepth;

  // struct tcl_syntax_block *blocks;
  // struct tcl_syntax_stmt *stmts;
  // struct tcl_syntax_word *words;
  // struct tcl_syntax_sub *subs;
  // char *subChars;

  // unsigned int blocksInd,blocksNum;
  // unsigned int stmtsInd,stmtsNum;
  // unsigned int wordsInd,wordsNum;
  // unsigned int subsInd,subsNum;
  // unsigned int subCharsInd,subCharsNum;
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
