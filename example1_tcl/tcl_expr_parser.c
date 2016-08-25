#include "tcl_expr_parser.h"
#include <stdio.h>

void tcl_expr_parser_on_lbracket(PARMAC_DEPTH stkDepth,
                       const char *machine,
                       const char *fromState,
                       const char *toState,
                       PARMAC_POS fromPos,
                       PARMAC_POS toPos,
                       void *userdata) {
  struct tcl_parser *tp=(struct tcl_parser*)userdata;
  printf("lbracket\n");
}

void tcl_expr_parser_on_rbracket(PARMAC_DEPTH stkDepth,
                       const char *machine,
                       const char *fromState,
                       const char *toState,
                       PARMAC_POS fromPos,
                       PARMAC_POS toPos,
                       void *userdata) {
  struct tcl_parser *tp=(struct tcl_parser*)userdata;
  printf("rbracket\n");
}

void tcl_expr_parser_on_int(PARMAC_DEPTH stkDepth,
                       const char *machine,
                       const char *fromState,
                       const char *toState,
                       PARMAC_POS fromPos,
                       PARMAC_POS toPos,
                       void *userdata) {
  struct tcl_parser *tp=(struct tcl_parser*)userdata;
  printf("int(%.*s)\n",(int)(toPos-fromPos),&tp->src[fromPos]);
}

void tcl_expr_parser_on_float(PARMAC_DEPTH stkDepth,
                         const char *machine,
                         const char *fromState,
                         const char *toState,
                         PARMAC_POS fromPos,
                         PARMAC_POS toPos,
                         void *userdata) {
  struct tcl_parser *tp=(struct tcl_parser*)userdata;
  printf("float(%.*s)\n",(int)(toPos-fromPos),&tp->src[fromPos]);
}

void tcl_expr_parser_on_unary_plus(PARMAC_DEPTH stkDepth,
                              const char *machine,
                              const char *fromState,
                              const char *toState,
                              PARMAC_POS fromPos,
                              PARMAC_POS toPos,
                              void *userdata) {
  struct tcl_parser *tp=(struct tcl_parser*)userdata;
  printf("unary_plus\n");
}

void tcl_expr_parser_on_unary_minus(PARMAC_DEPTH stkDepth,
                              const char *machine,
                              const char *fromState,
                              const char *toState,
                              PARMAC_POS fromPos,
                              PARMAC_POS toPos,
                              void *userdata) {
  struct tcl_parser *tp=(struct tcl_parser*)userdata;
  printf("unary_minus\n");
}

void tcl_expr_parser_on_unary_not(PARMAC_DEPTH stkDepth,
                              const char *machine,
                              const char *fromState,
                              const char *toState,
                              PARMAC_POS fromPos,
                              PARMAC_POS toPos,
                              void *userdata) {
  struct tcl_parser *tp=(struct tcl_parser*)userdata;
  printf("unary_not\n");
}

void tcl_expr_parser_on_binary_add(PARMAC_DEPTH stkDepth,
                              const char *machine,
                              const char *fromState,
                              const char *toState,
                              PARMAC_POS fromPos,
                              PARMAC_POS toPos,
                              void *userdata) {
  struct tcl_parser *tp=(struct tcl_parser*)userdata;
  printf("binary_add\n");
}

void tcl_expr_parser_on_binary_sub(PARMAC_DEPTH stkDepth,
                              const char *machine,
                              const char *fromState,
                              const char *toState,
                              PARMAC_POS fromPos,
                              PARMAC_POS toPos,
                              void *userdata) {
  struct tcl_parser *tp=(struct tcl_parser*)userdata;
  printf("binary_sub\n");
}


void tcl_expr_parser_on_binary_mul(PARMAC_DEPTH stkDepth,
                              const char *machine,
                              const char *fromState,
                              const char *toState,
                              PARMAC_POS fromPos,
                              PARMAC_POS toPos,
                              void *userdata) {
  struct tcl_parser *tp=(struct tcl_parser*)userdata;
  printf("binary_mul\n");
}

void tcl_expr_parser_on_binary_div(PARMAC_DEPTH stkDepth,
                              const char *machine,
                              const char *fromState,
                              const char *toState,
                              PARMAC_POS fromPos,
                              PARMAC_POS toPos,
                              void *userdata) {
  struct tcl_parser *tp=(struct tcl_parser*)userdata;
  printf("binary_div\n");
}

void tcl_expr_parser_on_binary_gt(PARMAC_DEPTH stkDepth,
                              const char *machine,
                              const char *fromState,
                              const char *toState,
                              PARMAC_POS fromPos,
                              PARMAC_POS toPos,
                              void *userdata) {
  struct tcl_parser *tp=(struct tcl_parser*)userdata;
  printf("binary_gt\n");
}

void tcl_expr_parser_on_binary_ge(PARMAC_DEPTH stkDepth,
                              const char *machine,
                              const char *fromState,
                              const char *toState,
                              PARMAC_POS fromPos,
                              PARMAC_POS toPos,
                              void *userdata) {
  struct tcl_parser *tp=(struct tcl_parser*)userdata;
  printf("binary_ge\n");
}

void tcl_expr_parser_on_binary_lt(PARMAC_DEPTH stkDepth,
                              const char *machine,
                              const char *fromState,
                              const char *toState,
                              PARMAC_POS fromPos,
                              PARMAC_POS toPos,
                              void *userdata) {
  struct tcl_parser *tp=(struct tcl_parser*)userdata;
  printf("binary_lt\n");
}

void tcl_expr_parser_on_binary_le(PARMAC_DEPTH stkDepth,
                              const char *machine,
                              const char *fromState,
                              const char *toState,
                              PARMAC_POS fromPos,
                              PARMAC_POS toPos,
                              void *userdata) {
  struct tcl_parser *tp=(struct tcl_parser*)userdata;
  printf("binary_le\n");
}

void tcl_expr_parser_on_binary_eq(PARMAC_DEPTH stkDepth,
                              const char *machine,
                              const char *fromState,
                              const char *toState,
                              PARMAC_POS fromPos,
                              PARMAC_POS toPos,
                              void *userdata) {
  struct tcl_parser *tp=(struct tcl_parser*)userdata;
  printf("binary_eq\n");
}

void tcl_expr_parser_on_binary_ne(PARMAC_DEPTH stkDepth,
                              const char *machine,
                              const char *fromState,
                              const char *toState,
                              PARMAC_POS fromPos,
                              PARMAC_POS toPos,
                              void *userdata) {
  struct tcl_parser *tp=(struct tcl_parser*)userdata;
  printf("binary_ne\n");
}

void tcl_expr_parser_on_binary_and(PARMAC_DEPTH stkDepth,
                              const char *machine,
                              const char *fromState,
                              const char *toState,
                              PARMAC_POS fromPos,
                              PARMAC_POS toPos,
                              void *userdata) {
  struct tcl_parser *tp=(struct tcl_parser*)userdata;
  printf("binary_and\n");
}

void tcl_expr_parser_on_binary_or(PARMAC_DEPTH stkDepth,
                              const char *machine,
                              const char *fromState,
                              const char *toState,
                              PARMAC_POS fromPos,
                              PARMAC_POS toPos,
                              void *userdata) {
  struct tcl_parser *tp=(struct tcl_parser*)userdata;
  printf("binary_or\n");
}

void tcl_expr_parser_on_binary_seq(PARMAC_DEPTH stkDepth,
                              const char *machine,
                              const char *fromState,
                              const char *toState,
                              PARMAC_POS fromPos,
                              PARMAC_POS toPos,
                              void *userdata) {
  struct tcl_parser *tp=(struct tcl_parser*)userdata;
  printf("binary_seq\n");
}

void tcl_expr_parser_on_binary_sne(PARMAC_DEPTH stkDepth,
                              const char *machine,
                              const char *fromState,
                              const char *toState,
                              PARMAC_POS fromPos,
                              PARMAC_POS toPos,
                              void *userdata) {
  struct tcl_parser *tp=(struct tcl_parser*)userdata;
  printf("binary_sne\n");
}

bool tcl_expr_parser_parse_comma(PARMAC_POS *ppos,void *userdata) {
  struct tcl_parser *tp=(struct tcl_parser*)userdata;

  if(tp->src[*ppos]==',') {
    tp->errMsg=NULL;
    (*ppos)++;
    return true;
  }

  return false;
}

bool tcl_expr_parser_parse_plus(PARMAC_POS *ppos,void *userdata) {
  struct tcl_parser *tp=(struct tcl_parser*)userdata;

  if(tp->src[*ppos]=='+') {
    tp->errMsg=NULL;
    (*ppos)++;
    return true;
  }

  return false;
}

bool tcl_expr_parser_parse_minus(PARMAC_POS *ppos,void *userdata) {
  struct tcl_parser *tp=(struct tcl_parser*)userdata;

  if(tp->src[*ppos]=='-') {
    tp->errMsg=NULL;
    (*ppos)++;
    return true;
  }

  return false;
}

bool tcl_expr_parser_parse_mul(PARMAC_POS *ppos,void *userdata) {
  struct tcl_parser *tp=(struct tcl_parser*)userdata;

  if(tp->src[*ppos]=='*') {
    tp->errMsg=NULL;
    (*ppos)++;
    return true;
  }

  return false;
}

bool tcl_expr_parser_parse_div(PARMAC_POS *ppos,void *userdata) {
  struct tcl_parser *tp=(struct tcl_parser*)userdata;

  if(tp->src[*ppos]=='/') {
    tp->errMsg=NULL;
    (*ppos)++;
    return true;
  }

  return false;
}

bool tcl_expr_parser_parse_eq(PARMAC_POS *ppos,void *userdata) {
  struct tcl_parser *tp=(struct tcl_parser*)userdata;

  if(tp->src[*ppos]=='=' && tp->src[*ppos+1]=='=') {
    tp->errMsg=NULL;
    (*ppos)+=2;
    return true;
  }

  return false;
}

bool tcl_expr_parser_parse_ne(PARMAC_POS *ppos,void *userdata) {
  struct tcl_parser *tp=(struct tcl_parser*)userdata;

  if(tp->src[*ppos]=='!' && tp->src[*ppos+1]=='=') {
    tp->errMsg=NULL;
    (*ppos)+=2;
    return true;
  }

  return false;
}

bool tcl_expr_parser_parse_gt(PARMAC_POS *ppos,void *userdata) {
  struct tcl_parser *tp=(struct tcl_parser*)userdata;

  if(tp->src[*ppos]=='>') {
    tp->errMsg=NULL;
    (*ppos)++;
    return true;
  }

  return false;
}

bool tcl_expr_parser_parse_lt(PARMAC_POS *ppos,void *userdata) {
  struct tcl_parser *tp=(struct tcl_parser*)userdata;

  if(tp->src[*ppos]=='<') {
    tp->errMsg=NULL;
    (*ppos)++;
    return true;
  }

  return false;
}

bool tcl_expr_parser_parse_le(PARMAC_POS *ppos,void *userdata) {
  struct tcl_parser *tp=(struct tcl_parser*)userdata;

  if(tp->src[*ppos]=='<' && tp->src[*ppos+1]=='=') {
    tp->errMsg=NULL;
    (*ppos)+=2;
    return true;
  }

  return false;
}

bool tcl_expr_parser_parse_ge(PARMAC_POS *ppos,void *userdata) {
  struct tcl_parser *tp=(struct tcl_parser*)userdata;

  if(tp->src[*ppos]=='>' && tp->src[*ppos+1]=='=') {
    tp->errMsg=NULL;
    (*ppos)+=2;
    return true;
  }

  return false;
}

bool tcl_expr_parser_parse_and(PARMAC_POS *ppos,void *userdata) {
  struct tcl_parser *tp=(struct tcl_parser*)userdata;

  if(tp->src[*ppos]=='&' && tp->src[*ppos+1]=='&') {
    tp->errMsg=NULL;
    (*ppos)+=2;
    return true;
  }

  return false;
}

bool tcl_expr_parser_parse_or(PARMAC_POS *ppos,void *userdata) {
  struct tcl_parser *tp=(struct tcl_parser*)userdata;

  if(tp->src[*ppos]=='|' && tp->src[*ppos+1]=='|') {
    tp->errMsg=NULL;
    (*ppos)+=2;
    return true;
  }

  return false;
}

bool tcl_expr_parser_parse_seq(PARMAC_POS *ppos,void *userdata) {
  struct tcl_parser *tp=(struct tcl_parser*)userdata;

  if(tp->src[*ppos]=='e' && tp->src[*ppos+1]=='q') {
    tp->errMsg=NULL;
    (*ppos)+=2;
    return true;
  }

  return false;
}

bool tcl_expr_parser_parse_sne(PARMAC_POS *ppos,void *userdata) {
  struct tcl_parser *tp=(struct tcl_parser*)userdata;

  if(tp->src[*ppos]=='n' && tp->src[*ppos+1]=='e') {
    tp->errMsg=NULL;
    (*ppos)+=2;
    return true;
  }

  return false;
}

bool tcl_expr_parser_parse_not(PARMAC_POS *ppos,void *userdata) {
  struct tcl_parser *tp=(struct tcl_parser*)userdata;

  if(tp->src[*ppos]=='!') {
    tp->errMsg=NULL;
    (*ppos)++;
    return true;
  }

  return false;
}

bool tcl_expr_parser_parse_whitespace(PARMAC_POS *ppos,void *userdata) {
  struct tcl_parser *tp=(struct tcl_parser*)userdata;
  PARMAC_POS start=*ppos;

  while(true) {
    if(tp->src[*ppos]=='\r' &&
       tp->src[*ppos+1]=='\n') {
      (*ppos)+=2;
    } else if(tp->src[*ppos]=='\\' &&
              tp->src[*ppos+1]=='\n') {
      (*ppos)+=2;
    } else if(tp->src[*ppos]=='\\' &&
              tp->src[*ppos+1]=='\r' &&
              tp->src[*ppos+2]=='\n') {
      (*ppos)+=3;
    } else if(tp->src[*ppos]=='\\' &&
              tp->src[*ppos+1]==' ') {
      (*ppos)+=2;
    } else if(tp->src[*ppos]==' ' ||
              tp->src[*ppos]=='\t' ||
              tp->src[*ppos]=='\n') {
      (*ppos)++;
    } else {
      break;
    }
  }

  if(start==*ppos) {
    return false;
  }

  tp->errMsg=NULL;
  return true;
}

bool tcl_expr_parser_parse_question(PARMAC_POS *ppos,void *userdata) {
  struct tcl_parser *tp=(struct tcl_parser*)userdata;

  if(tp->src[*ppos]=='?') {
    tp->errMsg=NULL;
    (*ppos)++;
    return true;
  }

  return false;
}

bool tcl_expr_parser_parse_colon(PARMAC_POS *ppos,void *userdata) {
  struct tcl_parser *tp=(struct tcl_parser*)userdata;

  if(tp->src[*ppos]==':') {
    tp->errMsg=NULL;
    (*ppos)++;
    return true;
  }

  return false;
}

bool tcl_expr_parser_parse_lbracket(PARMAC_POS *ppos,void *userdata) {
  struct tcl_parser *tp=(struct tcl_parser*)userdata;

  if(tp->src[*ppos]=='(') {
    tp->errMsg=NULL;
    (*ppos)++;
    return true;
  }

  return false;
}

bool tcl_expr_parser_parse_rbracket(PARMAC_POS *ppos,void *userdata) {
  struct tcl_parser *tp=(struct tcl_parser*)userdata;

  if(tp->src[*ppos]==')') {
    tp->errMsg=NULL;
    (*ppos)++;
    return true;
  }

  tp->errMsg="Expecting closing bracket.\n";
  return false;
}

bool tcl_expr_parser_parse_int(PARMAC_POS *ppos,void *userdata) {
  struct tcl_parser *tp=(struct tcl_parser*)userdata;
  PARMAC_POS start=*ppos;

  while(tp->src[*ppos]>='0' && tp->src[*ppos] <='9') {
    (*ppos)++;
  }

  if(start==*ppos) {
    return false;
  }

  tp->errMsg=NULL;
  return true;
}

bool tcl_expr_parser_parse_float(PARMAC_POS *ppos,void *userdata) {
  struct tcl_parser *tp=(struct tcl_parser*)userdata;
  PARMAC_POS start=*ppos;

  while(tp->src[*ppos]>='0' && tp->src[*ppos] <='9') {
    (*ppos)++;
  }

  if(tp->src[*ppos]=='.') {
    (*ppos)++;

    while(tp->src[*ppos]>='0' && tp->src[*ppos] <='9') {
      (*ppos)++;
    }
  }

  if(start==*ppos) {
    return false;
  }

  tp->errMsg=NULL;
  return true;
}

bool tcl_expr_parser_parse_func_cos(PARMAC_POS *ppos,void *userdata) {
  struct tcl_parser *tp=(struct tcl_parser*)userdata;
  PARMAC_POS start=*ppos;

  if(strcmp(&tp->src[*ppos],"cos")==0) {
    (*ppos)+=3;
    tp->errMsg=NULL;
    return true;
  }

  return false;
}

bool tcl_expr_parser_parse_func_fmod(PARMAC_POS *ppos,void *userdata) {
  struct tcl_parser *tp=(struct tcl_parser*)userdata;
  PARMAC_POS start=*ppos;

  if(strcmp(&tp->src[*ppos],"fmod")==0) {
    (*ppos)+=4;
    tp->errMsg=NULL;
    return true;
  }

  return false;
}

bool tcl_expr_parser_parse_func_max(PARMAC_POS *ppos,void *userdata) {
  struct tcl_parser *tp=(struct tcl_parser*)userdata;
  PARMAC_POS start=*ppos;

  if(strcmp(&tp->src[*ppos],"max")==0) {
    (*ppos)+=3;
    tp->errMsg=NULL;
    return true;
  }

  return false;
}

void tcl_expr_parser_main_machine(struct parmac *p,PARMAC_POS pos);

void tcl_expr_parser_func_args1(struct parmac *p,PARMAC_POS pos) {
  static const struct parmac_state
    state_start={"start", NULL,NULL, NULL,NULL},
    state_ws={"ws", tcl_expr_parser_parse_whitespace,NULL, NULL,NULL},
    state_lbracket={"lbracket", tcl_expr_parser_parse_lbracket,NULL, NULL,NULL},
    state_rbracket={"rbracket", tcl_expr_parser_parse_rbracket,NULL, NULL,NULL},
    state_arg={"arg", NULL,tcl_expr_parser_main_machine, NULL,NULL},
    state_end={"end",NULL,NULL, NULL,NULL};

  static const struct parmac_transition trsns[]={
    {&state_start, &state_ws},
    {&state_start, &state_lbracket},
    {&state_ws, &state_lbracket},
    {&state_lbracket, &state_arg},
    {&state_arg, &state_rbracket},
    {&state_rbracket, &state_end},
    PARMAC_TRANSITION_END};

  parmac_set(p,pos,"e_args1",&state_start,&state_end,trsns);
}

void tcl_expr_parser_func_args2(struct parmac *p,PARMAC_POS pos) {
  static const struct parmac_state
    state_start={"start", NULL,NULL, NULL,NULL},
    state_ws={"ws", tcl_expr_parser_parse_whitespace,NULL, NULL,NULL},
    state_lbracket={"lbracket", tcl_expr_parser_parse_lbracket,NULL, NULL,NULL},
    state_rbracket={"rbracket", tcl_expr_parser_parse_rbracket,NULL, NULL,NULL},
    state_comma={"comma", tcl_expr_parser_parse_comma,NULL, NULL,NULL},
    state_arg0={"arg0", NULL,tcl_expr_parser_main_machine, NULL,NULL},
    state_arg1={"arg1", NULL,tcl_expr_parser_main_machine, NULL,NULL},
    state_end={"end",NULL,NULL, NULL,NULL};

  static const struct parmac_transition trsns[]={
    {&state_start, &state_ws},
    {&state_start, &state_lbracket},
    {&state_ws, &state_lbracket},
    {&state_lbracket, &state_arg0},
    {&state_arg0, &state_comma},
    {&state_comma, &state_arg1},
    {&state_arg1, &state_rbracket},
    {&state_rbracket, &state_end},
    PARMAC_TRANSITION_END};

  parmac_set(p,pos,"e_args2",&state_start,&state_end,trsns);
}

void tcl_expr_parser_func_argsn(struct parmac *p,PARMAC_POS pos) {
  static const struct parmac_state
    state_start={"start", NULL,NULL, NULL,NULL},
    state_ws={"ws", tcl_expr_parser_parse_whitespace,NULL, NULL,NULL},
    state_lbracket={"lbracket", tcl_expr_parser_parse_lbracket,NULL, NULL,NULL},
    state_rbracket={"rbracket", tcl_expr_parser_parse_rbracket,NULL, NULL,NULL},
    state_comma={"comma", tcl_expr_parser_parse_comma,NULL, NULL,NULL},
    state_arg={"arg", NULL,tcl_expr_parser_main_machine, NULL,NULL},
    state_end={"end",NULL,NULL, NULL,NULL};

    static const struct parmac_transition trsns[]={
      {&state_start, &state_ws},
      {&state_start, &state_lbracket},
      {&state_ws, &state_lbracket},
      {&state_lbracket, &state_arg},
      {&state_arg, &state_comma},
      {&state_arg, &state_rbracket},
      {&state_comma, &state_arg},
      {&state_rbracket, &state_end},
      PARMAC_TRANSITION_END};

    parmac_set(p,pos,"e_argsn",&state_start,&state_end,trsns);
}

void tcl_expr_parser_func_machine(struct parmac *p,PARMAC_POS pos) {
  static const struct parmac_state
    state_start={"start", NULL,NULL, NULL,NULL},
    state_cos={"cos", tcl_expr_parser_parse_func_cos,NULL, NULL,NULL},
    state_fmod={"fmod", tcl_expr_parser_parse_func_fmod,NULL, NULL,NULL},
    state_max={"max", tcl_expr_parser_parse_func_max,NULL, NULL,NULL},
    state_args1={"args1", NULL,tcl_expr_parser_func_args1, NULL,NULL},
    state_args2={"args2", NULL,tcl_expr_parser_func_args2, NULL,NULL},
    state_argsn={"argsn", NULL,tcl_expr_parser_func_argsn, NULL,NULL},
    state_end={"end",NULL,NULL, NULL,NULL};

  static const struct parmac_transition trsns[]={
    {&state_start, &state_cos},
    {&state_start, &state_fmod},
    {&state_start, &state_max},
    {&state_cos, &state_args1},
    {&state_fmod, &state_args2},
    {&state_max, &state_argsn},
    {&state_args1, &state_end},
    {&state_args2, &state_end},
    {&state_argsn, &state_end},
    PARMAC_TRANSITION_END};

  parmac_set(p,pos,"e_func",&state_start,&state_end,trsns);
}

void tcl_expr_parser_bracketed_machine(struct parmac *p,PARMAC_POS pos) {
  static const struct parmac_state
    state_start={"start", NULL,NULL, NULL,NULL},
    state_lbracket={"lbracket",
                    tcl_expr_parser_parse_lbracket,NULL,
                    tcl_expr_parser_on_lbracket,NULL},
    state_rbracket={"rbracket",
                    tcl_expr_parser_parse_rbracket,NULL,
                    tcl_expr_parser_on_rbracket,NULL},
    state_main={"main",
                NULL,tcl_expr_parser_main_machine,
                NULL,NULL},
    state_end={"end",NULL,NULL, NULL,NULL};

  static const struct parmac_transition trsns[]={
    {&state_start, &state_lbracket},
    {&state_lbracket, &state_main},
    {&state_main, &state_rbracket},
    {&state_rbracket, &state_end},
    PARMAC_TRANSITION_END};

  parmac_set(p,pos,"e_bracketed",&state_start,&state_end,trsns);
}

void tcl_expr_parser_val_machine(struct parmac *p,PARMAC_POS pos) {
  static const struct parmac_state
    state_start={"start", NULL,NULL, NULL,NULL},
    state_int={"int", tcl_expr_parser_parse_int,NULL, tcl_expr_parser_on_int,NULL},
    state_float={"float", tcl_expr_parser_parse_float,NULL, tcl_expr_parser_on_float,NULL},
    state_bracketed={"bracketed", NULL,tcl_expr_parser_bracketed_machine, NULL,NULL},
    state_func={"func", NULL,tcl_expr_parser_func_machine, NULL,NULL},
    state_end={"end",NULL,NULL, NULL,NULL};

  static const struct parmac_transition trsns[]={
    {&state_start, &state_int},
    {&state_start, &state_float},
    {&state_start, &state_bracketed},
    {&state_start, &state_func},
    {&state_int, &state_end},
    {&state_float, &state_end},
    {&state_bracketed, &state_end},
    {&state_func, &state_end},
    PARMAC_TRANSITION_END};

  parmac_set(p,pos,"e_val",&state_start,&state_end,trsns);
}

void tcl_expr_parser_ternary_machine(struct parmac *p,PARMAC_POS pos) {
  static const struct parmac_state
    state_start={"start",NULL,NULL, NULL,NULL},
    state_question={"question",
                    tcl_expr_parser_parse_question,NULL,
                    NULL,NULL},
    state_colon={"colon",
                 tcl_expr_parser_parse_colon, NULL,
                 NULL,NULL},
    state_then={"then",
                NULL,tcl_expr_parser_main_machine,
                NULL,NULL},
    state_else={"else",
                NULL,tcl_expr_parser_main_machine,
                NULL,NULL},
    state_end={"end",NULL,NULL, NULL,NULL};

  static const struct parmac_transition trsns[]={
    {&state_start, &state_question},
    {&state_question, &state_then},
    {&state_then, &state_colon},
    {&state_colon, &state_else},
    {&state_else, &state_end},
    PARMAC_TRANSITION_END};

  parmac_set(p,pos,"e_ternary",&state_start,&state_end,trsns);
}

void tcl_expr_parser_unary_machine(struct parmac *p,PARMAC_POS pos) {
  static const struct parmac_state
    state_start={"start", NULL,NULL, NULL,NULL},
    state_plus={"plus",
                tcl_expr_parser_parse_plus,NULL,
                tcl_expr_parser_on_unary_plus,NULL},
    state_minus={"minus",
                 tcl_expr_parser_parse_minus,NULL,
                 tcl_expr_parser_on_unary_minus,NULL},
    state_not={"not",
               tcl_expr_parser_parse_not,NULL,
               tcl_expr_parser_on_unary_not,NULL},
    state_end={"end",NULL,NULL, NULL,NULL};

  static const struct parmac_transition trsns[]={
    {&state_start, &state_plus},
    {&state_start, &state_minus},
    {&state_start, &state_not},
    {&state_plus, &state_end},
    {&state_minus, &state_end},
    {&state_not, &state_end},
    PARMAC_TRANSITION_END};

  parmac_set(p,pos,"e_unnary",&state_start,&state_end,trsns);
}

void tcl_expr_parser_binary_machine(struct parmac *p,PARMAC_POS pos) {
  static const struct parmac_state
    state_start={"start", NULL,NULL, NULL,NULL},
    state_add={"add",
               tcl_expr_parser_parse_plus,NULL,
               tcl_expr_parser_on_binary_add,NULL},
    state_sub={"sub",
               tcl_expr_parser_parse_minus,NULL,
               tcl_expr_parser_on_binary_sub,NULL},
    state_mul={"mul",
               tcl_expr_parser_parse_mul,NULL,
               tcl_expr_parser_on_binary_mul,NULL},
    state_div={"div",
               tcl_expr_parser_parse_div,NULL,
               tcl_expr_parser_on_binary_div,NULL},
    state_gt={"gt",
              tcl_expr_parser_parse_gt,NULL,
              tcl_expr_parser_on_binary_gt,NULL},
    state_ge={"ge",
              tcl_expr_parser_parse_ge,NULL,
              tcl_expr_parser_on_binary_ge,NULL},
    state_lt={"lt",
              tcl_expr_parser_parse_lt,NULL,
              tcl_expr_parser_on_binary_lt,NULL},
    state_le={"le",
              tcl_expr_parser_parse_le,NULL,
              tcl_expr_parser_on_binary_le,NULL},
    state_eq={"eq",
              tcl_expr_parser_parse_eq,NULL,
              tcl_expr_parser_on_binary_eq,NULL},
    state_ne={"ne",
              tcl_expr_parser_parse_ne,NULL,
              tcl_expr_parser_on_binary_ne,NULL},
    state_seq={"seq",
               tcl_expr_parser_parse_seq,NULL,
               tcl_expr_parser_on_binary_seq,NULL},
    state_sne={"sne",
               tcl_expr_parser_parse_sne,NULL,
               tcl_expr_parser_on_binary_sne,NULL},
    state_end={"end",NULL,NULL, NULL,NULL};

  static const struct parmac_transition trsns[]={
    {&state_start, &state_add},
    {&state_start, &state_sub},
    {&state_start, &state_mul},
    {&state_start, &state_div},
    {&state_start, &state_gt},
    {&state_start, &state_ge},
    {&state_start, &state_lt},
    {&state_start, &state_le},
    {&state_start, &state_eq},
    {&state_start, &state_ne},
    {&state_start, &state_seq},
    {&state_start, &state_sne},
    {&state_add, &state_end},
    {&state_sub, &state_end},
    {&state_mul, &state_end},
    {&state_div, &state_end},
    {&state_gt, &state_end},
    {&state_ge, &state_end},
    {&state_lt, &state_end},
    {&state_le, &state_end},
    {&state_eq, &state_end},
    {&state_ne, &state_end},
    {&state_seq, &state_end},
    {&state_sne, &state_end},
    PARMAC_TRANSITION_END};

  parmac_set(p,pos,"e_binary",&state_start,&state_end,trsns);
}

void tcl_expr_parser_main_machine(struct parmac *p,PARMAC_POS pos) {
  static const struct parmac_state
    state_start={"start",NULL,NULL, NULL,NULL},
    state_ws1={"ws1", tcl_expr_parser_parse_whitespace,NULL, NULL,NULL},
    state_ws2={"ws2", tcl_expr_parser_parse_whitespace,NULL, NULL,NULL},
    state_val={"val", NULL,tcl_expr_parser_val_machine, NULL,NULL},
    state_unary={"unary", NULL,tcl_expr_parser_unary_machine, NULL,NULL},
    state_infix={"infix", NULL,tcl_expr_parser_binary_machine, NULL,NULL},
    state_ternary={"ternary", NULL,tcl_expr_parser_ternary_machine, NULL,NULL},
    state_end={"end",NULL,NULL, NULL,NULL};

  static const struct parmac_transition trsns[]={
    {&state_start, &state_ws1},
    {&state_start, &state_val,},
    {&state_start, &state_unary},
    {&state_ws1, &state_unary},
    {&state_ws1, &state_val},
    {&state_unary, &state_val},
    {&state_unary, &state_ws1},
    {&state_unary, &state_unary},
    {&state_val, &state_ws2},
    {&state_val, &state_infix},
    {&state_val, &state_ternary},
    {&state_val, &state_end},
    {&state_infix, &state_ws1},
    {&state_infix, &state_unary},
    {&state_infix, &state_val},
    {&state_ws2, &state_infix},
    {&state_ws2, &state_ternary},
    {&state_ws2, &state_end},
    {&state_ternary, &state_end},
    PARMAC_TRANSITION_END};

  parmac_set(p,pos,"e_main",&state_start,&state_end,trsns);
}
