#ifndef TCL_PARSER_H
#define TCL_PARSER_H

#ifndef __cplusplus
#include <stdbool.h>
#endif

#include <string.h>
#include <stdlib.h>
#include "tcl_syntax.h"
#include "../../parmac.h"


struct tcl_parser {
  unsigned int closingsInd,stkNum;
  char *closings;
  struct parmac *stk;

  unsigned int depth;
  const char *errMsg;
  const char *src;
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
