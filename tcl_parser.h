#ifndef LANG_PARSER_H
#define LANG_PARSER_H

#ifndef __cplusplus
#include <stdbool.h>
#endif

// #define LANG_DEBUG_PARSE_FSM 1

#define LANG_USE_VARS
#define LANG_USE_CMDS
#define LANG_USE_QSTRS
#define LANG_USE_BSTRS
#define LANG_USE_SSTRS

#include "parmac.h"


struct lang_syntax_stmt;

enum lang_syntax_sub_type {
  lang_syntax_sub_str,
  lang_syntax_sub_var,
  lang_syntax_sub_stmt
};

struct lang_syntax_sub {
  enum lang_syntax_sub_type type;
  char *valStr;
  struct lang_syntax_stmt *valStmt;
  struct lang_syntax_sub *next;
};

struct lang_syntax_word {
  struct lang_syntax_sub *subs;
  struct lang_syntax_word *next;
};

struct lang_syntax_stmt {
  struct lang_syntax_word *words;
  struct lang_syntax_stmt *next;
};


struct lang_parser_syntax_builder {
  struct lang_syntax_stmt *rootStmt, *lastStmt;
  struct lang_syntax_word *rootWord, *lastWord;
  struct lang_syntax_sub *rootSub, *lastSub;
  const char *markStart,*markEnd;
};

// struct lang_parser_syntax_builder *builder

#ifdef __cplusplus
extern "C" {
#endif
void lang_parser_init(struct parmac **p, const char *src);

#ifdef __cplusplus
}
#endif

#endif
