#include "tcl_parser.h"

#include <stdio.h>
#include <stddef.h>

void tcl_parser_on_esc_a(PARMAC_DEPTH stkDepth,
                         const char *machine,
                         const char *fromState,
                         const char *toState,
                         PARMAC_POS fromPos,
                         PARMAC_POS toPos,
                         void *data) {
  struct tcl_parser *tp=(struct tcl_parser*)data;

  tcl_syntax_push(tp->syntax,tp->depth,
                  (unsigned int)fromPos,
                  tcl_syntax_str,
                  "\a",1);
}

void tcl_parser_on_esc_b(PARMAC_DEPTH stkDepth,
                         const char *machine,
                         const char *fromState,
                         const char *toState,
                         PARMAC_POS fromPos,
                         PARMAC_POS toPos,
                         void *data) {
  struct tcl_parser *tp=(struct tcl_parser*)data;

  tcl_syntax_push(tp->syntax,tp->depth,
                  (unsigned int)fromPos,
                  tcl_syntax_str,
                  "\b",1);
}

void tcl_parser_on_esc_f(PARMAC_DEPTH stkDepth,
                         const char *machine,
                         const char *fromState,
                         const char *toState,
                         PARMAC_POS fromPos,
                         PARMAC_POS toPos,
                         void *data) {
  struct tcl_parser *tp=(struct tcl_parser*)data;

  tcl_syntax_push(tp->syntax,tp->depth,
                  (unsigned int)fromPos,
                  tcl_syntax_str,
                  "\f",1);
}

void tcl_parser_on_esc_n(PARMAC_DEPTH stkDepth,
                         const char *machine,
                         const char *fromState,
                         const char *toState,
                         PARMAC_POS fromPos,
                         PARMAC_POS toPos,
                         void *data) {
  struct tcl_parser *tp=(struct tcl_parser*)data;

  tcl_syntax_push(tp->syntax,tp->depth,
                  (unsigned int)fromPos,
                  tcl_syntax_str,
                  "\n",1);
}

void tcl_parser_on_esc_r(PARMAC_DEPTH stkDepth,
                         const char *machine,
                         const char *fromState,
                         const char *toState,
                         PARMAC_POS fromPos,
                         PARMAC_POS toPos,
                         void *data) {
  struct tcl_parser *tp=(struct tcl_parser*)data;

  tcl_syntax_push(tp->syntax,tp->depth,
                  (unsigned int)fromPos,
                  tcl_syntax_str,
                  "\r",1);
}

void tcl_parser_on_esc_t(PARMAC_DEPTH stkDepth,
                         const char *machine,
                         const char *fromState,
                         const char *toState,
                         PARMAC_POS fromPos,
                         PARMAC_POS toPos,
                         void *data) {
  struct tcl_parser *tp=(struct tcl_parser*)data;

  tcl_syntax_push(tp->syntax,tp->depth,
                  (unsigned int)fromPos,
                  tcl_syntax_str,
                  "\t",1);
}

void tcl_parser_on_esc_v(PARMAC_DEPTH stkDepth,
                         const char *machine,
                         const char *fromState,
                         const char *toState,
                         PARMAC_POS fromPos,
                         PARMAC_POS toPos,
                         void *data) {
  struct tcl_parser *tp=(struct tcl_parser*)data;

  tcl_syntax_push(tp->syntax,tp->depth,
                  (unsigned int)fromPos,
                  tcl_syntax_str,
                  "\v",1);
}

void tcl_parser_on_esc_eol(PARMAC_DEPTH stkDepth,
                           const char *machine,
                           const char *fromState,
                           const char *toState,
                           PARMAC_POS fromPos,
                           PARMAC_POS toPos,
                           void *data) {
  struct tcl_parser *tp=(struct tcl_parser*)data;

  tcl_syntax_push(tp->syntax,tp->depth,
                  (unsigned int)fromPos,
                  tcl_syntax_str,
                  " ",1);
}

void tcl_parser_on_esc_char(PARMAC_DEPTH stkDepth,
                            const char *machine,
                            const char *fromState,
                            const char *toState,
                            PARMAC_POS fromPos,
                            PARMAC_POS toPos,
                            void *data) {

  struct tcl_parser *tp=(struct tcl_parser*)data;

  tcl_syntax_push(tp->syntax,tp->depth,
                  (unsigned int)fromPos,
                  tcl_syntax_str,
                  &tp->src[fromPos+1],1);
}

void tcl_parser_on_str(PARMAC_DEPTH stkDepth,
                       const char *machine,
                       const char *fromState,
                       const char *toState,
                       PARMAC_POS fromPos,
                       PARMAC_POS toPos,
                       void *data) {

  struct tcl_parser *tp=(struct tcl_parser*)data;

  tcl_syntax_push(tp->syntax,tp->depth,
                  (unsigned int)fromPos,
                  tcl_syntax_str,
                  &tp->src[fromPos],
                  (unsigned int)(toPos-fromPos));
}

void tcl_parser_on_var(PARMAC_DEPTH stkDepth,
                       const char *machine,
                       const char *fromState,
                       const char *toState,
                       PARMAC_POS fromPos,
                       PARMAC_POS toPos,
                       void *userdata) {
  struct tcl_parser *tp=(struct tcl_parser*)userdata;

  //set
  tcl_syntax_push(tp->syntax,tp->depth+1,
                  (unsigned int)fromPos,
                  tcl_syntax_str,
                  "set",3);

  //var_name
  tcl_syntax_push(tp->syntax,tp->depth+1,
                  (unsigned int)fromPos,
                  tcl_syntax_str,
                  &tp->src[fromPos],
                  (unsigned int)(toPos-fromPos));

  //sep
  tcl_syntax_push(tp->syntax,tp->depth+1,
                  (unsigned int)toPos,
                  tcl_syntax_sep,
                  NULL,0);
}

void tcl_parser_after_word(PARMAC_DEPTH stkDepth,
                        const char *machine,
                        const char *fromState,
                        const char *toState,
                        PARMAC_POS fromPos,
                        PARMAC_POS toPos,
                        void *userdata) {
  struct tcl_parser *tp=(struct tcl_parser*)userdata;

  tcl_syntax_push(tp->syntax,tp->depth,
                  (unsigned int)toPos,
                  tcl_syntax_spc,
                  NULL,0);

}

void tcl_parser_after_stmt(PARMAC_DEPTH stkDepth,
                        const char *machine,
                        const char *fromState,
                        const char *toState,
                        PARMAC_POS fromPos,
                        PARMAC_POS toPos,
                        void *userdata) {
  struct tcl_parser *tp=(struct tcl_parser*)userdata;

  tcl_syntax_push(tp->syntax,tp->depth,
                  (unsigned int)toPos,
                  tcl_syntax_sep,
                  NULL,0);
}

void tcl_parser_on_enter_cmd(PARMAC_DEPTH stkDepth,
                             const char *machine,
                             const char *fromState,
                             const char *toState,
                             PARMAC_POS fromPos,
                             PARMAC_POS toPos,
                             void *userdata) {

  struct tcl_parser *tp=(struct tcl_parser*)userdata;
  tp->depth++;
}

void tcl_parser_on_leave_cmd(PARMAC_DEPTH stkDepth,
                             const char *machine,
                             const char *fromState,
                             const char *toState,
                             PARMAC_POS fromPos,
                             PARMAC_POS toPos,
                             void *userdata) {

  struct tcl_parser *tp=(struct tcl_parser*)userdata;
  tp->depth--;
}

bool tcl_parser_parse_esc_a(PARMAC_POS *ppos,void *userdata) {
  struct tcl_parser *tp=(struct tcl_parser*)userdata;

  if(tp->src[*ppos]=='\\' && tp->src[*ppos+1]=='a') {
    (*ppos)+=2;
    tp->errMsg=NULL;
    return true;
  }

  return false;
}

bool tcl_parser_parse_esc_b(PARMAC_POS *ppos,void *userdata) {
  struct tcl_parser *tp=(struct tcl_parser*)userdata;

  if(tp->src[*ppos]=='\\' && tp->src[*ppos+1]=='b') {
    (*ppos)+=2;
    tp->errMsg=NULL;
    return true;
  }

  return false;
}

bool tcl_parser_parse_esc_f(PARMAC_POS *ppos,void *userdata) {
  struct tcl_parser *tp=(struct tcl_parser*)userdata;

  if(tp->src[*ppos]=='\\' && tp->src[*ppos+1]=='f') {
    (*ppos)+=2;
    tp->errMsg=NULL;
    return true;
  }

  return false;
}

bool tcl_parser_parse_esc_n(PARMAC_POS *ppos,void *userdata) {
  struct tcl_parser *tp=(struct tcl_parser*)userdata;

  if(tp->src[*ppos]=='\\' && tp->src[*ppos+1]=='n') {
    (*ppos)+=2;
    tp->errMsg=NULL;
    return true;
  }

  return false;
}

bool tcl_parser_parse_esc_r(PARMAC_POS *ppos,void *userdata) {
  struct tcl_parser *tp=(struct tcl_parser*)userdata;

  if(tp->src[*ppos]=='\\' && tp->src[*ppos+1]=='r') {
    (*ppos)+=2;
    tp->errMsg=NULL;
    return true;
  }

  return false;
}

bool tcl_parser_parse_esc_t(PARMAC_POS *ppos,void *userdata) {
  struct tcl_parser *tp=(struct tcl_parser*)userdata;

  if(tp->src[*ppos]=='\\' && tp->src[*ppos+1]=='t') {
    (*ppos)+=2;
    tp->errMsg=NULL;
    return true;
  }

  return false;
}

bool tcl_parser_parse_esc_v(PARMAC_POS *ppos,void *userdata) {
  struct tcl_parser *tp=(struct tcl_parser*)userdata;

  if(tp->src[*ppos]=='\\' && tp->src[*ppos+1]=='v') {
    (*ppos)+=2;
    tp->errMsg=NULL;
    return true;
  }

  return false;
}

bool tcl_parser_parse_esc_any(PARMAC_POS *ppos,void *userdata) {
  struct tcl_parser *tp=(struct tcl_parser*)userdata;

  if(tp->src[*ppos]=='\\' && tp->src[*ppos+1]!='\0') {
    (*ppos)+=2;
    tp->errMsg=NULL;
    return true;
  }

  return false;
}

bool tcl_parser_parse_esc_eol(PARMAC_POS *ppos,void *userdata) {
  struct tcl_parser *tp=(struct tcl_parser*)userdata;

  if(tp->src[*ppos]=='\\' && tp->src[*ppos+1]=='\n') {
    (*ppos)+=2;
    tp->errMsg=NULL;
    return true;
  }

  if(tp->src[*ppos]=='\\' &&
     tp->src[*ppos+1]=='\r' &&
     tp->src[*ppos+2]=='\n') {
    (*ppos)+=3;
    tp->errMsg=NULL;
    return true;
  }

  return false;
}

bool tcl_parser_parse_sep(PARMAC_POS *ppos,void *userdata) {
  struct tcl_parser *tp=(struct tcl_parser*)userdata;
  PARMAC_POS start=*ppos;

  while(true) {
    if(tp->src[*ppos]=='\r' && tp->src[*ppos+1]=='\n') {
      (*ppos)+=2;
    } else if(tp->src[*ppos]=='\n' || tp->src[*ppos]==';') {
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

bool tcl_parser_parse_spc0(PARMAC_POS *ppos,void *userdata) {
  struct tcl_parser *tp=(struct tcl_parser*)userdata;
  PARMAC_POS start=*ppos;

  while(true) {
    if(tp->src[*ppos]==' ' ||
       tp->src[*ppos]=='\t') {
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

bool tcl_parser_parse_spcn(PARMAC_POS *ppos,void *userdata) {
  struct tcl_parser *tp=(struct tcl_parser*)userdata;
  PARMAC_POS start=*ppos;

  while(true) {
    if(tp->src[*ppos]==' ' ||
       tp->src[*ppos]=='\t') {
      (*ppos)++;
    } else if(tp->src[*ppos]=='\\' &&
              tp->src[*ppos+1]=='\n') {
      (*ppos)+=2;
    } else if(tp->src[*ppos]=='\\' &&
              tp->src[*ppos+1]=='\r' &&
              tp->src[*ppos+2]=='\n') {
      (*ppos)+=3;
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

bool tcl_parser_parse_cmnt(PARMAC_POS *ppos,void *userdata) {
  struct tcl_parser *tp=(struct tcl_parser*)userdata;

  if(tp->src[*ppos] != '#') {
    return false;
  }

  (*ppos)++;

  while((tp->src[*ppos] != '\r' && tp->src[*ppos+1] != '\n') &&
        tp->src[*ppos] != '\n' &&
        tp->src[*ppos] != '\0') {
   (*ppos)++;
  }

  //unnecessary but cleaner...
  if(tp->src[*ppos] != '\r' && tp->src[*ppos+1] != '\n') {
    (*ppos)+=2;
  } else if(tp->src[*ppos] != '\n') {
   (*ppos)++;
  }

  //
  tp->errMsg=NULL;
  return true;
}

bool tcl_parser_parse_lquote(PARMAC_POS *ppos,void *userdata) {
  struct tcl_parser *tp=(struct tcl_parser*)userdata;

  if(tp->src[*ppos]!='"') {
    return false;
  }

  (*ppos)++;
  tp->errMsg=NULL;
  return true;
}

bool tcl_parser_parse_rquote(PARMAC_POS *ppos,void *userdata) {
  struct tcl_parser *tp=(struct tcl_parser*)userdata;

  if(tp->src[*ppos]!='"') {
    tp->errMsg="Expecting closing double quote.\n";
    return false;
  }

  (*ppos)++;
  tp->errMsg=NULL;
  return true;
}

bool tcl_parser_parse_lsqr(PARMAC_POS *ppos,void *userdata) {
  struct tcl_parser *tp=(struct tcl_parser*)userdata;

  if(tp->src[*ppos]!='[') {
    return false;
  }

  (*ppos)++;
  tp->errMsg=NULL;
  tp->sqrbCount++;
  return true;
}

bool tcl_parser_parse_rsqr(PARMAC_POS *ppos,void *userdata) {
  struct tcl_parser *tp=(struct tcl_parser*)userdata;

  if(tp->src[*ppos]!=']') {
    tp->errMsg="Expecting closing square bracket.\n";
    return false;
  }

  (*ppos)++;
  tp->errMsg=NULL;
  tp->sqrbCount--;
  return true;
}

bool tcl_parser_parse_lbrace(PARMAC_POS *ppos,void *userdata) {
  struct tcl_parser *tp=(struct tcl_parser*)userdata;

  if(tp->src[*ppos]!='{') {
    return false;
  }

  (*ppos)++;
  tp->errMsg=NULL;
  return true;
}

bool tcl_parser_parse_rbrace(PARMAC_POS *ppos,void *userdata) {
  struct tcl_parser *tp=(struct tcl_parser*)userdata;

  if(tp->src[*ppos]!='}') {
    tp->errMsg="Expecting closing curly brace.\n";
    return false;
  }

  (*ppos)++;
  tp->errMsg=NULL;
  return true;
}

bool tcl_parser_parse_sstr(PARMAC_POS *ppos,void *userdata) {
  struct tcl_parser *tp=(struct tcl_parser*)userdata;
  PARMAC_POS start=*ppos;

  while(true) {
    if(tp->src[*ppos]==' ' ||
       tp->src[*ppos]=='\t' ||
       tp->src[*ppos]==';' ||
       tp->src[*ppos]=='[' ||
       tp->src[*ppos]=='\\' ||
       tp->src[*ppos]=='\0') {
      break;
    }

    if(tp->src[*ppos]=='\n' ||
       (tp->src[*ppos]=='\r' && tp->src[*ppos+1]=='\n')) {
      break;
    }

    if(tp->src[*ppos]=='\\' &&
       tp->src[*ppos+1]=='\r' &&
       tp->src[*ppos+2]=='\n') {
      break;
    }

    if(tp->src[*ppos]=='\\' && tp->src[*ppos+1]=='\n') {
      break;
    }

    if(tp->sqrbCount!=0 && tp->src[*ppos]==']') {
      break;
    }

    if(tp->src[*ppos]=='$' &&
       (tp->src[*ppos+1]=='{' ||
        tp->src[*ppos+1]=='_' ||
        tp->src[*ppos+1]=='(' ||
        (tp->src[*ppos+1]>='a' && tp->src[*ppos+1]<='z') ||
        (tp->src[*ppos+1]>='A' && tp->src[*ppos+1]<='Z') ||
        (tp->src[*ppos+1]>='0' && tp->src[*ppos+1]<='9'))) {

      break;
    }

    (*ppos)++;
  }

  if(start==*ppos) {
    return false;
  }

  tp->errMsg=NULL;
  return true;
}

bool tcl_parser_parse_qstr(PARMAC_POS *ppos,void *userdata) {
  struct tcl_parser *tp=(struct tcl_parser*)userdata;
  PARMAC_POS start=*ppos;

  while(true) {
    if(tp->src[*ppos]=='"' ||
       tp->src[*ppos]=='[' ||
       tp->src[*ppos]=='\\' ||
       tp->src[*ppos]=='\0') {
      break;
    }

    if(tp->src[*ppos]=='$') {
      if(tp->src[*ppos+1]=='{' ||
         tp->src[*ppos+1]=='_' ||
         tp->src[*ppos+1]=='(') {
        break;
      }

      if((tp->src[*ppos+1]>='a' && tp->src[*ppos+1]<='z') ||
         (tp->src[*ppos+1]>='A' && tp->src[*ppos+1]<='Z') ||
         (tp->src[*ppos+1]>='0' && tp->src[*ppos+1]<='9')) {
        break;
      }
    }

    (*ppos)++;
  }

  if(start==*ppos) {
    return false;
  }

  tp->errMsg=NULL;
  return true;
}

bool tcl_parser_parse_bstr(PARMAC_POS *ppos,void *userdata) {
  struct tcl_parser *tp=(struct tcl_parser*)userdata;
  PARMAC_POS start=*ppos;
  unsigned int c=0;

  while(true) {
    if(c==0 && tp->src[*ppos]=='}') {
      break;
    }

    if(tp->src[*ppos]=='\0') {
      break;
    }

    if(tp->src[*ppos]=='{') {
      c++;
    } else if(tp->src[*ppos]=='}') {
      c--;
    }

    if(tp->src[*ppos]=='\\' && tp->src[*ppos+1]=='{') {
      (*ppos)+=2;
    } else if(tp->src[*ppos]=='\\' && tp->src[*ppos+1]=='}') {
      (*ppos)+=2;
    } else {
      (*ppos)++;
    }
  }

  if(start==*ppos) {
    return false;
  }

  tp->errMsg=NULL;
  return true;
}

bool tcl_parser_parse_var_str(PARMAC_POS *ppos,void *userdata) {
  struct tcl_parser *tp=(struct tcl_parser*)userdata;
  PARMAC_POS start=*ppos;
  unsigned int c=0;

  while(true) {
    if(c==0 && tp->src[*ppos]=='}') {
      break;
    }

    if(tp->src[*ppos]=='\0') {
      break;
    }

    if(tp->src[*ppos]=='{') {
      c++;
    } else if(tp->src[*ppos]=='}') {
      c--;
    }

    (*ppos)++;
  }

  if(start==*ppos) {
    return false;
  }

  tp->errMsg=NULL;
  return true;
}

bool tcl_parser_parse_var_idn(PARMAC_POS *ppos,void *userdata) {
  struct tcl_parser *tp=(struct tcl_parser*)userdata;
  PARMAC_POS start=*ppos;

  while(tp->src[*ppos]=='_' ||
        (tp->src[*ppos]>='a' && tp->src[*ppos]<='z') ||
        (tp->src[*ppos]>='A' && tp->src[*ppos]<='Z') ||
        (tp->src[*ppos]>='0' && tp->src[*ppos]<='9')) {
    (*ppos)++;
  }

  if(tp->src[*ppos]=='(') {
    while(tp->src[*ppos]!=')') {
      if(tp->src[*ppos]=='\0') {
        tp->errMsg="Expecting closing bracket.";
        return false;
      }

      (*ppos)++;
    }

    (*ppos)++;
  }

  if(start==*ppos) {
    return false;
  }

  tp->errMsg=NULL;
  return true;
}

bool tcl_parser_parse_dollar(PARMAC_POS *ppos,void *userdata) {
  struct tcl_parser *tp=(struct tcl_parser*)userdata;

  if(tp->src[*ppos]!='$') {
    return false;
  }

  (*ppos)++;
  tp->errMsg=NULL;
  return true;
}

void tcl_parser_main_machine(struct parmac *p,PARMAC_POS pos);

void tcl_parser_cmd_machine(struct parmac *p,PARMAC_POS pos) {
  static const struct parmac_state
    state_start={"start", NULL,NULL, tcl_parser_on_enter_cmd,NULL},
    state_lsqr={"lsqr", tcl_parser_parse_lsqr,NULL, NULL,NULL},
    state_rsqr={"rsqr", tcl_parser_parse_rsqr,NULL, NULL,NULL},
    state_main={"main", NULL,tcl_parser_main_machine, NULL,NULL},
    state_end={"end", NULL,NULL, tcl_parser_on_leave_cmd,NULL};

  static const struct parmac_transition trsns[]={
    {&state_start, &state_lsqr},
    {&state_lsqr, &state_rsqr},
    {&state_lsqr, &state_main},
    {&state_main, &state_rsqr},
    {&state_rsqr, &state_end},
    PARMAC_TRANSITION_END};

  parmac_set(p,pos,"t_cmd",&state_start,&state_end,trsns);
}

void tcl_parser_var_machine(struct parmac *p,PARMAC_POS pos) {
  static const struct parmac_state
    state_start={"start", NULL,NULL, NULL,NULL},
    state_dollar={"dollar", tcl_parser_parse_dollar,NULL, NULL,NULL},
    state_idn={"idn", tcl_parser_parse_var_idn,NULL, tcl_parser_on_var,NULL},
    state_str={"str", tcl_parser_parse_var_str,NULL, tcl_parser_on_var,NULL},
    state_lbrace={"lbrace", tcl_parser_parse_lbrace,NULL, NULL,NULL},
    state_rbrace={"rbrace", tcl_parser_parse_rbrace,NULL, NULL,NULL},
    state_end={"end", NULL,NULL, NULL,NULL};

  static const struct parmac_transition trsns[]={
    {&state_start, &state_dollar},
    {&state_dollar, &state_lbrace},
    {&state_dollar, &state_idn},
    {&state_lbrace, &state_rbrace},
    {&state_lbrace, &state_str},
    {&state_str, &state_rbrace},
    {&state_rbrace, &state_end},
    {&state_idn, &state_end},
    PARMAC_TRANSITION_END};

  parmac_set(p,pos,"t_var",&state_start,&state_end,trsns);
}

void tcl_parser_sesc_machine(struct parmac *p,PARMAC_POS pos) {
  static const struct parmac_state
    state_start={"start", NULL,NULL, NULL,NULL},
    state_a={"a", tcl_parser_parse_esc_a,NULL, tcl_parser_on_esc_a,NULL},
    state_b={"b", tcl_parser_parse_esc_b,NULL, tcl_parser_on_esc_b,NULL},
    state_f={"f", tcl_parser_parse_esc_f,NULL, tcl_parser_on_esc_f,NULL},
    state_n={"n", tcl_parser_parse_esc_n,NULL, tcl_parser_on_esc_n,NULL},
    state_r={"r", tcl_parser_parse_esc_r,NULL, tcl_parser_on_esc_r,NULL},
    state_t={"t", tcl_parser_parse_esc_t,NULL, tcl_parser_on_esc_t,NULL},
    state_v={"v", tcl_parser_parse_esc_v,NULL, tcl_parser_on_esc_v,NULL},
    state_any={"any", tcl_parser_parse_esc_any,NULL, tcl_parser_on_esc_char,NULL},
    state_end={"end", NULL,NULL, NULL,NULL};

  static const struct parmac_transition trsns[]={
    {&state_start, &state_a},
    {&state_start, &state_b},
    {&state_start, &state_f},
    {&state_start, &state_n},
    {&state_start, &state_r},
    {&state_start, &state_t},
    {&state_start, &state_v},
    {&state_start, &state_any},
    {&state_a, &state_end},
    {&state_b, &state_end},
    {&state_f, &state_end},
    {&state_n, &state_end},
    {&state_r, &state_end},
    {&state_t, &state_end},
    {&state_v, &state_end},
    {&state_any, &state_end},
    PARMAC_TRANSITION_END};

  parmac_set(p,pos,"t_sesc",&state_start,&state_end,trsns);
}

void tcl_parser_sstr_machine(struct parmac *p,PARMAC_POS pos) {
  static const struct parmac_state
    state_start={"start", NULL,NULL, NULL,NULL},
    state_sstr={"sstr", tcl_parser_parse_sstr,NULL, tcl_parser_on_str,NULL},
    state_sesc={"sesc", NULL,tcl_parser_sesc_machine, NULL,NULL},
    state_var={"var", NULL,tcl_parser_var_machine, NULL,NULL},
    state_cmd={"cmd", NULL,tcl_parser_cmd_machine, NULL,NULL},
    state_end={"end", NULL,NULL, NULL,NULL};

  static const struct parmac_transition trsns[]={
    {&state_start, &state_sesc},
    {&state_start, &state_sstr},
    {&state_start, &state_var},
    {&state_start, &state_cmd},
    {&state_var, &state_sesc},
    {&state_var, &state_sstr},
    {&state_var, &state_var},
    {&state_var, &state_cmd},
    {&state_var, &state_end},
    {&state_cmd, &state_sesc},
    {&state_cmd, &state_sstr},
    {&state_cmd, &state_var},
    {&state_cmd, &state_cmd},
    {&state_cmd, &state_end},
    {&state_sstr, &state_sesc},
    {&state_sstr, &state_var},
    {&state_sstr, &state_cmd},
    {&state_sstr, &state_end},
    {&state_sesc, &state_sesc},
    {&state_sesc, &state_sstr},
    {&state_sesc, &state_var},
    {&state_sesc, &state_cmd},
    {&state_sesc, &state_end},
    PARMAC_TRANSITION_END};

  parmac_set(p,pos,"t_sstr",&state_start,&state_end,trsns);
}

void tcl_parser_qesc_machine(struct parmac *p,PARMAC_POS pos) {
  static const struct parmac_state
    state_start={"start", NULL,NULL, NULL,NULL},
    state_eol={"eol", tcl_parser_parse_esc_eol,NULL, tcl_parser_on_esc_eol,NULL},
    state_sesc={"sesc", NULL,tcl_parser_sesc_machine, NULL,NULL},
    state_end={"end", NULL,NULL, NULL,NULL};

  static const struct parmac_transition trsns[]={
    {&state_start, &state_eol},
    {&state_start, &state_sesc},
    {&state_eol, &state_end},
    {&state_sesc, &state_end},
    PARMAC_TRANSITION_END};

  parmac_set(p,pos,"t_qesc",&state_start,&state_end,trsns);
}

void tcl_parser_qstr_machine(struct parmac *p,PARMAC_POS pos) {
  static const struct parmac_state
    state_start={"start", NULL,NULL, NULL,NULL},
    state_lquote={"lquote", tcl_parser_parse_lquote,NULL, NULL,NULL},
    state_rquote={"rquote", tcl_parser_parse_rquote,NULL, NULL,NULL},
    state_qstr={"qstr", tcl_parser_parse_qstr,NULL, tcl_parser_on_str,NULL},
    state_qesc={"qesc", NULL,tcl_parser_qesc_machine, NULL,NULL},
    state_var={"var", NULL,tcl_parser_var_machine, NULL,NULL},
    state_cmd={"cmd", NULL,tcl_parser_cmd_machine, NULL,NULL},
    state_end={"end", NULL,NULL, NULL,NULL};

  static const struct parmac_transition trsns[]={
    {&state_start, &state_lquote},
    {&state_lquote, &state_rquote},
    {&state_lquote, &state_qesc},
    {&state_lquote, &state_qstr},
    {&state_lquote, &state_var},
    {&state_lquote, &state_cmd},
    {&state_var, &state_rquote},
    {&state_var, &state_qesc},
    {&state_var, &state_qstr},
    {&state_var, &state_var},
    {&state_var, &state_cmd},
    {&state_cmd, &state_rquote},
    {&state_cmd, &state_qesc},
    {&state_cmd, &state_qstr},
    {&state_cmd, &state_var},
    {&state_cmd, &state_cmd},
    {&state_qstr, &state_rquote},
    {&state_qstr, &state_qesc},
    {&state_qstr, &state_var},
    {&state_qstr, &state_cmd},
    {&state_qesc, &state_rquote},
    {&state_qesc, &state_qesc},
    {&state_qesc, &state_qstr},
    {&state_qesc, &state_var},
    {&state_qesc, &state_cmd},
    {&state_rquote, &state_end},
    PARMAC_TRANSITION_END};

  parmac_set(p,pos,"t_qstr",&state_start,&state_end,trsns);
}

void tcl_parser_bstr_machine(struct parmac *p,PARMAC_POS pos) {
  static const struct parmac_state
    state_start={"start", NULL,NULL, NULL,NULL},
    state_lbrace={"lbrace", tcl_parser_parse_lbrace,NULL, NULL,NULL},
    state_rbrace={"rbrace", tcl_parser_parse_rbrace,NULL, NULL,NULL},
    state_bstr={"bstr", tcl_parser_parse_bstr,NULL, tcl_parser_on_str,NULL},
    state_end={"end", NULL,NULL, NULL,NULL};

  static const struct parmac_transition trsns[]={
    {&state_start, &state_lbrace},
    {&state_lbrace, &state_rbrace},
    {&state_lbrace, &state_bstr},
    {&state_bstr, &state_rbrace},
    {&state_rbrace, &state_end},
    PARMAC_TRANSITION_END};

  parmac_set(p,pos,"t_bstr",&state_start,&state_end,trsns);
}

void tcl_parser_word_machine(struct parmac *p,PARMAC_POS pos) {
  static const struct parmac_state
    state_start={"start", NULL,NULL, NULL,NULL},
    state_bstr={"bstr", NULL,tcl_parser_bstr_machine, NULL,NULL},
    state_qstr={"qstr", NULL,tcl_parser_qstr_machine, NULL,NULL},
    state_sstr={"sstr", NULL,tcl_parser_sstr_machine, NULL,NULL},
    state_end={"end", NULL,NULL, NULL,NULL};

  static const struct parmac_transition trsns[]={
    {&state_start, &state_qstr},
    {&state_start, &state_bstr},
    {&state_start, &state_sstr},
    {&state_qstr, &state_end},
    {&state_bstr, &state_end},
    {&state_sstr, &state_end},
    PARMAC_TRANSITION_END};

  parmac_set(p,pos,"t_word",&state_start,&state_end,trsns);
}

void tcl_parser_main_machine(struct parmac *p,PARMAC_POS pos) {
  static const struct parmac_state
    state_start={"start", NULL,NULL, NULL,NULL},
    state_cmnt={"cmnt", tcl_parser_parse_cmnt,NULL, NULL,NULL},
    state_word={"word", NULL,tcl_parser_word_machine, tcl_parser_after_word,NULL},
    state_spc0={"spc0", tcl_parser_parse_spc0,NULL, NULL,NULL},
    state_spcn={"spcn", tcl_parser_parse_spcn,NULL, NULL,NULL},
    state_sep={"sep", tcl_parser_parse_sep,NULL, tcl_parser_after_stmt,NULL},
    state_end={"end", NULL,NULL, tcl_parser_after_stmt,NULL};

  static const struct parmac_transition trsns[]={
    {&state_start, &state_cmnt},
    {&state_start, &state_sep},
    {&state_start, &state_spc0},
    {&state_start, &state_word},
    {&state_start, &state_end},
    {&state_cmnt, &state_sep},
    {&state_cmnt, &state_end},
    {&state_word, &state_spcn},
    {&state_word, &state_sep},
    {&state_word, &state_end},
    {&state_spc0, &state_cmnt},
    {&state_spc0, &state_sep},
    {&state_spc0, &state_word},
    {&state_spc0, &state_end},
    {&state_spcn, &state_sep},
    {&state_spcn, &state_word},
    {&state_spcn, &state_end},
    {&state_sep, &state_cmnt},
    {&state_sep, &state_spc0},
    {&state_sep, &state_word},
    {&state_sep, &state_end},
    PARMAC_TRANSITION_END};

  parmac_set(p,pos,"t_main",&state_start,&state_end,trsns);
}
