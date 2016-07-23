#ifndef TCL_PARSER_H
#define TCL_PARSER_H

#ifndef __cplusplus
#include <stdbool.h>
#endif

#include "old0_parmac.h"

struct tcl_parser {
  const char *errMsg;
  const char *markStart,*markEnd;
  int pos,row,col;
};

#ifdef __cplusplus
extern "C" {
#endif

  void tcl_parser_main_machine(struct parmac *p,const char *src);

#ifdef __cplusplus
}
#endif

#endif
