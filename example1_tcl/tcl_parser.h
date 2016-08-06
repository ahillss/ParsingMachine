#ifndef TCL_PARSER_H
#define TCL_PARSER_H

#ifndef __cplusplus
#include <stdbool.h>
#endif

#include <string.h>
#include <stdlib.h>
#include "tcl_syntax.h"
#include "../parmac.h"
#include "tcl_parser_util.h"

struct tcl_parser {
  struct tcl_syntax *syntax;
  const char *errMsg;

  const char *src;
  unsigned int depth,sqrbCount;

  unsigned int stkNum;
  struct parmac *stk;
};

#ifdef __cplusplus
extern "C" {
#endif

  void tcl_parser_init(struct tcl_parser *tp);
  void tcl_parser_uninit(struct tcl_parser *tp);
  void tcl_parser_run(struct tcl_parser *tp,struct tcl_syntax *syntax,const char *src);

#ifdef __cplusplus
}
#endif

#endif
