#ifndef TCL_PARSER_UTIL_H
#define TCL_PARSER_UTIL_H

#ifndef __cplusplus
#include <stdbool.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

  void tcl_parser_util_printSyntaxError(const char *src,const char *srcPos,const char *errMsg);

#ifdef __cplusplus
}
#endif

#endif
