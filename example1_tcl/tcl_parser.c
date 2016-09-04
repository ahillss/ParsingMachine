#include "tcl_parser.h"

#include <stdio.h>
#include <stddef.h>

void tcl_parser_on_esc_char(PARMAC_DEPTH stkDepth,
                            const char *machine,
                            const char *fromState,
                            const char *toState,
                            PARMAC_POS fromPos,
                            PARMAC_POS toPos,
                            void *data) {
  static const char cs[256]={
    0,1,2,3,4,5,6,7,8,9,' ',11,12,13,14,15,16,17,18,19,20,21,22,23,
    24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,
    45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,
    66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,
    87,88,89,90,91,92,93,94,95,96,'\a','\b',99,100,101,'\f',103,104,
    105,106,107,108,109,'\n',111,112,113,'\r',115,'\t',117,'\v',119,
    120,121,122,123,124,125,126,127,128,129,130,131,132,133,134,135,
    136,137,138,139,140,141,142,143,144,145,146,147,148,149,150,151,
    152,153,154,155,156,157,158,159,160,161,162,163,164,165,166,167,
    168,169,170,171,172,173,174,175,176,177,178,179,180,181,182,183,
    184,185,186,187,188,189,190,191,192,193,194,195,196,197,198,199,
    200,201,202,203,204,205,206,207,208,209,210,211,212,213,214,215,
    216,217,218,219,220,221,222,223,224,225,226,227,228,229,230,231,
    232,233,234,235,236,237,238,239,240,241,242,243,244,245,246,247,
    248,249,250,251,252,253,254,255};

  struct tcl_parser *tp=(struct tcl_parser*)data;

  tcl_syntax_push(tp->syntax,tp->depth,
                  (unsigned int)fromPos,
                  tcl_syntax_str,
                  &cs[tp->src[toPos-1]],1);
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

  //spc
  tcl_syntax_push(tp->syntax,tp->depth+1,
                  (unsigned int)toPos,
                  tcl_syntax_spc,
                  NULL,0);

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

bool tcl_parser_parse_esc_schar(PARMAC_POS *ppos,void *userdata) {
  struct tcl_parser *tp=(struct tcl_parser*)userdata;

  if(tp->src[*ppos]!='\\') {
    return false;
  }

  if(tp->src[*ppos+1]!='\0') {
    (*ppos)+=2;
    tp->errMsg=NULL;
    return true;
  }

  return false;
}

bool tcl_parser_parse_esc_qchar(PARMAC_POS *ppos,void *userdata) {
  struct tcl_parser *tp=(struct tcl_parser*)userdata;

  if(tp->src[*ppos]!='\\') {
    return false;
  }

  if(tp->src[*ppos+1]=='\r' && tp->src[*ppos+2]=='\n') {
    (*ppos)+=3;
    tp->errMsg=NULL;
    return true;
  }

  if(tp->src[*ppos+1]!='\0') {
    (*ppos)+=2;
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
  // PARMAC_POS start=*ppos;

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

  // if(start==*ppos) {
  //   return false;
  // }

  tp->errMsg=NULL;
  return true;
}

bool tcl_parser_parse_vstr(PARMAC_POS *ppos,void *userdata) {
  struct tcl_parser *tp=(struct tcl_parser*)userdata;
  // PARMAC_POS start=*ppos;

  while(tp->src[*ppos]!='}') {
    (*ppos)++;
  }

  // if(start==*ppos) {
  //   return false;
  // }

  tp->errMsg=NULL;
  return true;
}

bool tcl_parser_parse_bstr(PARMAC_POS *ppos,void *userdata) {
  struct tcl_parser *tp=(struct tcl_parser*)userdata;
  // PARMAC_POS start=*ppos;
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

  // if(start==*ppos) {
  //   return false;
  // }

  tp->errMsg=NULL;
  return true;
}

bool tcl_parser_parse_idn(PARMAC_POS *ppos,void *userdata) {
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
    state_idn={"idn", tcl_parser_parse_idn,NULL, tcl_parser_on_var,NULL},
    state_str={"str", tcl_parser_parse_vstr,NULL, tcl_parser_on_var,NULL},
    state_lbrace={"lbrace", tcl_parser_parse_lbrace,NULL, NULL,NULL},
    state_rbrace={"rbrace", tcl_parser_parse_rbrace,NULL, NULL,NULL},
    state_end={"end", NULL,NULL, NULL,NULL};

  static const struct parmac_transition trsns[]={
    {&state_start, &state_dollar},
    {&state_dollar, &state_lbrace},
    {&state_dollar, &state_idn},
    // {&state_lbrace, &state_rbrace},
    {&state_lbrace, &state_str},
    {&state_str, &state_rbrace},
    {&state_rbrace, &state_end},
    {&state_idn, &state_end},
    PARMAC_TRANSITION_END};

  parmac_set(p,pos,"t_var",&state_start,&state_end,trsns);
}

void tcl_parser_sstr_machine(struct parmac *p,PARMAC_POS pos) {
  static const struct parmac_state
    state_start={"start", NULL,NULL, NULL,NULL},
    state_schr={"schr", tcl_parser_parse_sstr,NULL, tcl_parser_on_str,NULL},
    state_sesc={"sesc", tcl_parser_parse_esc_schar,NULL, tcl_parser_on_esc_char,NULL},
    state_var={"var", NULL,tcl_parser_var_machine, NULL,NULL},
    state_cmd={"cmd", NULL,tcl_parser_cmd_machine, NULL,NULL},
    state_end={"end", NULL,NULL, NULL,NULL};

  static const struct parmac_transition trsns[]={
    {&state_start, &state_sesc},
    {&state_start, &state_schr},
    {&state_start, &state_var},
    {&state_start, &state_cmd},
    {&state_var, &state_sesc},
    {&state_var, &state_schr},
    {&state_var, &state_var},
    {&state_var, &state_cmd},
    {&state_var, &state_end},
    {&state_cmd, &state_sesc},
    {&state_cmd, &state_schr},
    {&state_cmd, &state_var},
    {&state_cmd, &state_cmd},
    {&state_cmd, &state_end},
    {&state_schr, &state_sesc},
    {&state_schr, &state_var},
    {&state_schr, &state_cmd},
    {&state_schr, &state_end},
    {&state_sesc, &state_sesc},
    {&state_sesc, &state_schr},
    {&state_sesc, &state_var},
    {&state_sesc, &state_cmd},
    {&state_sesc, &state_end},
    PARMAC_TRANSITION_END};

  parmac_set(p,pos,"t_sstr",&state_start,&state_end,trsns);
}

void tcl_parser_qstr_machine(struct parmac *p,PARMAC_POS pos) {
  static const struct parmac_state
    state_start={"start", NULL,NULL, NULL,NULL},
    state_lquote={"lquote", tcl_parser_parse_lquote,NULL, NULL,NULL},
    state_rquote={"rquote", tcl_parser_parse_rquote,NULL, NULL,NULL},
    state_qchr={"qchr", tcl_parser_parse_qstr,NULL, tcl_parser_on_str,NULL},
    state_qesc={"qesc", tcl_parser_parse_esc_schar,NULL, tcl_parser_on_esc_char,NULL},
    state_var={"var", NULL,tcl_parser_var_machine, NULL,NULL},
    state_cmd={"cmd", NULL,tcl_parser_cmd_machine, NULL,NULL},
    state_end={"end", NULL,NULL, NULL,NULL};

  static const struct parmac_transition trsns[]={
    {&state_start, &state_lquote},
    // {&state_lquote, &state_rquote},
    {&state_lquote, &state_qesc},
    {&state_lquote, &state_var},
    {&state_lquote, &state_cmd},
    {&state_lquote, &state_qchr},
    {&state_var, &state_rquote},
    {&state_var, &state_qesc},
    {&state_var, &state_var},
    {&state_var, &state_cmd},
    {&state_var, &state_qchr},
    {&state_cmd, &state_rquote},
    {&state_cmd, &state_qesc},
    {&state_cmd, &state_var},
    {&state_cmd, &state_cmd},
    {&state_cmd, &state_qchr},
    {&state_qchr, &state_rquote},
    {&state_qchr, &state_qesc},
    {&state_qchr, &state_var},
    {&state_qchr, &state_cmd},
    {&state_qesc, &state_rquote},
    {&state_qesc, &state_qesc},
    {&state_qesc, &state_var},
    {&state_qesc, &state_cmd},
    {&state_qesc, &state_qchr},
    {&state_rquote, &state_end},
    PARMAC_TRANSITION_END};

  parmac_set(p,pos,"t_qstr",&state_start,&state_end,trsns);
}

void tcl_parser_bstr_machine(struct parmac *p,PARMAC_POS pos) {
  static const struct parmac_state
    state_start={"start", NULL,NULL, NULL,NULL},
    state_lbrace={"lbrace", tcl_parser_parse_lbrace,NULL, NULL,NULL},
    state_rbrace={"rbrace", tcl_parser_parse_rbrace,NULL, NULL,NULL},
    state_bchr={"bchr", tcl_parser_parse_bstr,NULL, tcl_parser_on_str,NULL},
    state_end={"end", NULL,NULL, NULL,NULL};

  static const struct parmac_transition trsns[]={
    {&state_start, &state_lbrace},
    // {&state_lbrace, &state_rbrace},
    {&state_lbrace, &state_bchr},
    {&state_bchr, &state_rbrace},
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
