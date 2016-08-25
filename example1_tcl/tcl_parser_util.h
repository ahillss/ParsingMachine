#ifndef TCL_PARSER_UTIL_H
#define TCL_PARSER_UTIL_H

#ifndef __cplusplus
#include <stdbool.h>
#endif

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

  void tcl_parser_util_printSyntaxError(const char *src,const char *srcPos,const char *errMsg);

#ifdef __cplusplus
}
#endif

#endif
