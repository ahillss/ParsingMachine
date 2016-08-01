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
  unsigned int stkNum,depth,sqrbCount;
  struct parmac *stk;
  const char *errMsg,*src;
  struct tcl_syntax *syntax;
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
