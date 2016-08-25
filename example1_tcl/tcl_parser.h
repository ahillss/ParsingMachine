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


#ifdef __cplusplus
extern "C" {
#endif

  void tcl_parser_main_machine(struct parmac *p,PARMAC_POS pos);
  void tcl_parser_qstr_machine(struct parmac *p,PARMAC_POS pos);
  void tcl_parser_bstr_machine(struct parmac *p,PARMAC_POS pos);
  void tcl_parser_cmd_machine(struct parmac *p,PARMAC_POS pos);

#ifdef __cplusplus
}
#endif

#endif
