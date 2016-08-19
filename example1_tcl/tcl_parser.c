#include "tcl_parser.h"

#include <stdio.h>
#include <stddef.h>

#define endof(x) (x+sizeof(x)/sizeof(*x))

char tcl_parser_convert_schar(const char *src) {


}

void tcl_parser_on_str(PARMAC_DEPTH stkDepth,
                       const char *machine,
                       const char *fromState,
                       const char *toState,
                       PARMAC_POS fromPos,
                       PARMAC_POS toPos,
                       void *data) {
  struct tcl_parser *tp=(struct tcl_parser*)data;

  struct tcl_syntax_node *n;
  n=tcl_syntax_push(tp->syntax,tp->depth,
                    (unsigned int)fromPos,
                    tcl_syntax_str);

  const char *c;
  char *s;
  c=&tp->src[fromPos];

  while(c!=&tp->src[toPos]) {
    s=tcl_syntax_str_push(tp->syntax,1);
    n->charsNum+=1;

    if(c[0]=='\\' && (c[1]=='\n' || (c[1]=='\r' && c[2]=='\n'))) {
      //for qstr only, not sstr
      s[0]=' ';
      c+=(c[1]=='\n')?2:3;
    } else if(c[0]=='\\' && c[1]!='\0') {
      if(c[1]=='a') {
        s[0]='\a';
      } else if(c[1]=='b') {
        s[0]='\b';
      } else if(c[1]=='f') {
        s[0]='\f';
      } else if(c[1]=='n') {
        s[0]='\n';
      } else if(c[1]=='r') {
        s[0]='\r';
      } else if(c[1]=='t') {
        s[0]='\t';
      } else if(c[1]=='v') {
        s[0]='\v';
      } else {
        s[0]=c[1];
      }

      c+=2;
    } else {
      s[0]=c[0];
      c++;
    }
  }

  s=tcl_syntax_str_push(tp->syntax,1);
  s[0]='\0';

}

void tcl_parser_on_bstr(PARMAC_DEPTH stkDepth,
                        const char *machine,
                        const char *fromState,
                        const char *toState,
                        PARMAC_POS fromPos,
                        PARMAC_POS toPos,
                        void *userdata) {
  struct tcl_parser *tp=(struct tcl_parser*)userdata;

  struct tcl_syntax_node *n;
  n=tcl_syntax_push(tp->syntax,tp->depth,
                    (unsigned int)fromPos,
                    tcl_syntax_str);

  char *s;
  const char *c;
  c=&tp->src[fromPos];

  while(c!=&tp->src[toPos]) {
    s=tcl_syntax_str_push(tp->syntax,1);
    n->charsNum+=1;

    if(c[0]=='\\' && (c[1]=='\n' || (c[1]=='\r' && c[2]=='\n'))) {
      s[0]=' ';
      c+=(c[1]=='\n')?2:3;
    } else {
      s[0]=c[0];
      c++;
    }
  }


  s=tcl_syntax_str_push(tp->syntax,1);
  s[0]='\0';

}

void tcl_parser_on_var(PARMAC_DEPTH stkDepth,
                       const char *machine,
                       const char *fromState,
                       const char *toState,
                       PARMAC_POS fromPos,
                       PARMAC_POS toPos,
                       void *userdata) {
  struct tcl_parser *tp=(struct tcl_parser*)userdata;

  struct tcl_syntax_node *n;
  char *s;

  //set
  n=tcl_syntax_push(tp->syntax,tp->depth+1,
                    (unsigned int)fromPos,
                    tcl_syntax_str);
  n->charsNum=3;
  s=tcl_syntax_str_push(tp->syntax,4);
  sprintf(s,"set");

  //
  tcl_syntax_push(tp->syntax,tp->depth+1,
                 (unsigned int)toPos,
                  tcl_syntax_spc);

  //var_name
  n=tcl_syntax_push(tp->syntax,tp->depth+1,
                    (unsigned int)fromPos,
                    tcl_syntax_str);
  n->charsNum=(unsigned int)(toPos-fromPos);
  s=tcl_syntax_str_push(tp->syntax,
                        n->charsNum+1);
  sprintf(s,"%.*s",n->charsNum,&tp->src[fromPos]);

  //
  tcl_syntax_push(tp->syntax,tp->depth+1,
                  (unsigned int)toPos,
                  tcl_syntax_sep);

}

void tcl_parser_on_word(PARMAC_DEPTH stkDepth,
                        const char *machine,
                        const char *fromState,
                        const char *toState,
                        PARMAC_POS fromPos,
                        PARMAC_POS toPos,
                        void *userdata) {
  struct tcl_parser *tp=(struct tcl_parser*)userdata;

  tcl_syntax_push(tp->syntax,tp->depth,
                  (unsigned int)toPos,
                  tcl_syntax_spc);

}

void tcl_parser_on_stmt(PARMAC_DEPTH stkDepth,
                        const char *machine,
                        const char *fromState,
                        const char *toState,
                        PARMAC_POS fromPos,
                        PARMAC_POS toPos,
                        void *userdata) {
  struct tcl_parser *tp=(struct tcl_parser*)userdata;

  tcl_syntax_push(tp->syntax,tp->depth,
                  (unsigned int)toPos,
                  tcl_syntax_sep);
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

bool tcl_parser_parse_sep(PARMAC_POS *ppos,void *userdata) {
  struct tcl_parser *tp=(struct tcl_parser*)userdata;
  PARMAC_POS start=*ppos;

  while(tp->src[*ppos]==';') {
    (*ppos)++;
  }

  if(start==*ppos) {
    return false;
  }

  tp->errMsg=NULL;
  return true;
}

bool tcl_parser_parse_eol(PARMAC_POS *ppos,void *userdata) {
  struct tcl_parser *tp=(struct tcl_parser*)userdata;
  PARMAC_POS start=*ppos;

  while(true) {
    if(tp->src[*ppos]=='\r' && tp->src[*ppos+1]=='\n') {
      (*ppos)+=2;
    } else if(tp->src[*ppos]=='\n') {
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

bool tcl_parser_parse_spc(PARMAC_POS *ppos,void *userdata) {
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
  PARMAC_POS start=*ppos;

  while(tp->src[*ppos]==' ' ||
        tp->src[*ppos]=='\t') {
    (*ppos)++;
  }

  if(tp->src[*ppos] != '#') {
    return false;
  }

  (*ppos)++;

  while((tp->src[*ppos] != '\r' && tp->src[*ppos+1] != '\n') &&
        tp->src[*ppos] != '\n' &&
        tp->src[*ppos] != '\0') {
   (*ppos)++;
  }

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

    if(tp->src[*ppos]=='\\' && tp->src[*ppos+1]!='\0') {
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

bool tcl_parser_parse_qstr(PARMAC_POS *ppos,void *userdata) {
  struct tcl_parser *tp=(struct tcl_parser*)userdata;
  PARMAC_POS start=*ppos;

  while(true) {
    if(tp->src[*ppos]=='"' ||
       tp->src[*ppos]=='[' ||
       tp->src[*ppos]=='\0') {
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

    if(tp->src[*ppos]=='\\' && tp->src[*ppos+1]=='"') {
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

    if(tp->src[*ppos]=='\\' && tp->src[*ppos+1]=='}') {
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
    {NULL,NULL}};

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
    {NULL,NULL}};

  parmac_set(p,pos,"t_var",&state_start,&state_end,trsns);
}

void tcl_parser_sstr_machine(struct parmac *p,PARMAC_POS pos) {
  static const struct parmac_state
    state_start={"start", NULL,NULL, NULL,NULL},
    state_sstr={"sstr", tcl_parser_parse_sstr,NULL, tcl_parser_on_str,NULL},
    state_var={"var", NULL,tcl_parser_var_machine, NULL,NULL},
    state_cmd={"cmd", NULL,tcl_parser_cmd_machine, NULL,NULL},
    state_end={"end", NULL,NULL, NULL,NULL};

  static const struct parmac_transition trsns[]={
    {&state_start, &state_sstr},
    {&state_start, &state_var},
    {&state_start, &state_cmd},
    {&state_var, &state_sstr},
    {&state_var, &state_var},
    {&state_var, &state_cmd},
    {&state_var, &state_end},
    {&state_cmd, &state_sstr},
    {&state_cmd, &state_var},
    {&state_cmd, &state_cmd},
    {&state_cmd, &state_end},
    {&state_sstr, &state_var},
    {&state_sstr, &state_cmd},
    {&state_sstr, &state_end},
    {NULL,NULL}};

  parmac_set(p,pos,"t_sstr",&state_start,&state_end,trsns);
}

void tcl_parser_qstr_machine(struct parmac *p,PARMAC_POS pos) {
  static const struct parmac_state
    state_start={"start", NULL,NULL, NULL,NULL},
    state_lquote={"lquote", tcl_parser_parse_lquote,NULL, NULL,NULL},
    state_rquote={"rquote", tcl_parser_parse_rquote,NULL, NULL,NULL},
    state_qstr={"qstr", tcl_parser_parse_qstr,NULL, tcl_parser_on_str,NULL},
    state_var={"var", NULL,tcl_parser_var_machine, NULL,NULL},
    state_cmd={"cmd", NULL,tcl_parser_cmd_machine, NULL,NULL},
    state_end={"end", NULL,NULL, NULL,NULL};

  static const struct parmac_transition trsns[]={
    {&state_start, &state_lquote},
    {&state_lquote, &state_rquote},
    {&state_lquote, &state_qstr},
    {&state_lquote, &state_var},
    {&state_lquote, &state_cmd},
    {&state_var, &state_rquote},
    {&state_var, &state_qstr},
    {&state_var, &state_var},
    {&state_var, &state_cmd},
    {&state_cmd, &state_rquote},
    {&state_cmd, &state_qstr},
    {&state_cmd, &state_var},
    {&state_cmd, &state_cmd},
    {&state_qstr, &state_rquote},
    {&state_qstr, &state_var},
    {&state_qstr, &state_cmd},
    {&state_rquote, &state_end},
    {NULL,NULL}};

  parmac_set(p,pos,"t_qstr",&state_start,&state_end,trsns);
}

void tcl_parser_bstr_machine(struct parmac *p,PARMAC_POS pos) {
  static const struct parmac_state
    state_start={"start", NULL,NULL, NULL,NULL},
    state_lbrace={"lbrace", tcl_parser_parse_lbrace,NULL, NULL,NULL},
    state_rbrace={"rbrace", tcl_parser_parse_rbrace,NULL, NULL,NULL},
    state_bstr={"bstr", tcl_parser_parse_bstr,NULL, tcl_parser_on_bstr,NULL},
    state_end={"end", NULL,NULL, NULL,NULL};

  static const struct parmac_transition trsns[]={
    {&state_start, &state_lbrace},
    {&state_lbrace, &state_rbrace},
    {&state_lbrace, &state_bstr},
    {&state_bstr, &state_rbrace},
    {&state_rbrace, &state_end},
    {NULL,NULL}};

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
    {NULL,NULL}};

  parmac_set(p,pos,"t_word",&state_start,&state_end,trsns);
}

void tcl_parser_main_machine(struct parmac *p,PARMAC_POS pos) {
  static const struct parmac_state
    state_start={"start", NULL,NULL, NULL,NULL},
    state_cmnt={"cmnt", tcl_parser_parse_cmnt,NULL, NULL,NULL},
    state_word={"word", NULL,tcl_parser_word_machine, tcl_parser_on_word,NULL},
    state_spc={"spc", tcl_parser_parse_spc,NULL, NULL,NULL},
    state_sep={"sep", tcl_parser_parse_sep,NULL, tcl_parser_on_stmt,NULL},
    state_eol={"eol", tcl_parser_parse_eol,NULL, tcl_parser_on_stmt,NULL},
    state_end={"end", NULL,NULL, tcl_parser_on_stmt,NULL};

  static const struct parmac_transition trsns[]={
    {&state_start, &state_cmnt},
    {&state_start, &state_sep},
    {&state_start, &state_eol},
    {&state_start, &state_spc},
    {&state_start, &state_word},
    {&state_start, &state_end},
    {&state_cmnt, &state_eol},
    {&state_cmnt, &state_end},
    {&state_word, &state_spc},
    {&state_word, &state_sep},
    {&state_word, &state_eol},
    {&state_word, &state_end},
    {&state_spc, &state_sep},
    {&state_spc, &state_eol},
    {&state_spc, &state_word},
    {&state_spc, &state_end},
    {&state_sep, &state_cmnt},
    {&state_sep, &state_spc},
    {&state_sep, &state_eol},
    {&state_sep, &state_word},
    {&state_sep, &state_end},
    {&state_eol, &state_cmnt},
    {&state_eol, &state_spc},
    {&state_eol, &state_sep},
    {&state_eol, &state_word},
    {&state_eol, &state_end},
    {NULL,NULL}};

  parmac_set(p,pos,"t_main",&state_start,&state_end,trsns);
}

void tcl_parser_init(struct tcl_parser *tp) {

  tp->stkNum=2;
  tp->stk=(struct parmac*)malloc(sizeof(struct parmac)*tp->stkNum);


}

void tcl_parser_uninit(struct tcl_parser *tp) {
  free(tp->stk);
}


void tcl_parser_run(struct tcl_parser *tp,
                    struct tcl_syntax *syntax,
                    const char *src) {
  PARMAC_DEPTH stkDepth=0;

  tp->depth=0;
  tp->errMsg=NULL;
  tp->src=src;
  tp->syntax=syntax;
  tp->sqrbCount=0;

  tcl_parser_main_machine(tp->stk,0);

  while(parmac_run(tp->stk,&stkDepth,tp)) {
    if(stkDepth+1==tp->stkNum) {
      tp->stkNum*=2;
      tp->stk=(struct parmac*)realloc(tp->stk,sizeof(struct parmac)*tp->stkNum);
    }
  }

  if(parmac_failed(tp->stk) || (tp->src[parmac_last_pos(tp->stk,stkDepth)]!='\0')) {
    printf("Error.\n");

    if(tp->errMsg!=NULL) {
      printf(tp->errMsg);
    }

    tcl_parser_util_printSyntaxError(src,&tp->src[parmac_last_pos(tp->stk,stkDepth)],tp->errMsg);

  } else {

  }
}
