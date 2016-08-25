#ifndef TCL_EXPR_PARSER_H
#define TCL_EXPR_PARSER_H

#ifndef __cplusplus
#include <stdbool.h>
#endif

#include <string.h>
#include <stdlib.h>
#include "../parmac.h"
// #include "tcl_expr_syntax.h"
#include "tcl_parser_util.h"

struct tcl_expr_parser {
  const char *src,*errMsg;
};

#ifdef __cplusplus
extern "C" {
#endif
  void tcl_expr_parser_main_machine(struct parmac *p,PARMAC_POS pos);


#ifdef __cplusplus
}
#endif

#endif
