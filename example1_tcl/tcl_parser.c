#include "tcl_parser.h"

#include <stdio.h>
#include <stddef.h>

#define endof(x) (x+sizeof(x)/sizeof(*x))

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
    state_start={"start",tcl_parser_on_enter_cmd,NULL},
    state_lsqr={"lsqr",NULL,NULL},
    state_rsqr={"rsqr",NULL,NULL},
    state_main={"main",NULL,NULL},
    state_end={"end",tcl_parser_on_leave_cmd,NULL};

  static const struct parmac_transition trsns[]={
    {&state_start, &state_lsqr, tcl_parser_parse_lsqr, NULL},

    {&state_lsqr, &state_rsqr, tcl_parser_parse_rsqr, NULL},
    {&state_lsqr, &state_main, NULL, tcl_parser_main_machine},

    {&state_main, &state_rsqr, tcl_parser_parse_rsqr, NULL},

    {&state_rsqr, &state_end, NULL, NULL}
  };

  parmac_set(p,"tcmd",pos,&state_start,&state_end,trsns, endof(trsns));
}

void tcl_parser_var_machine(struct parmac *p,PARMAC_POS pos) {
  static const struct parmac_state
    state_start={"start",NULL,NULL},
    state_dollar={"dollar",NULL,NULL},
    state_idn={"idn",tcl_parser_on_var,NULL},
    state_str={"str",tcl_parser_on_var,NULL},
    state_lbrace={"lbrace",NULL,NULL},
    state_rbrace={"rbrace",NULL,NULL},
    state_end={"end",NULL,NULL};

  static const struct parmac_transition trsns[]={
    {&state_start, &state_dollar, tcl_parser_parse_dollar, NULL},

    {&state_dollar, &state_lbrace, tcl_parser_parse_lbrace, NULL},
    {&state_dollar, &state_idn, tcl_parser_parse_var_idn, NULL},

    {&state_lbrace, &state_rbrace, tcl_parser_parse_rbrace,  NULL},
    {&state_lbrace, &state_str, tcl_parser_parse_var_str, NULL},

    {&state_str, &state_rbrace, tcl_parser_parse_rbrace, NULL},

    {&state_rbrace, &state_end, NULL, NULL},

    {&state_idn, &state_end, NULL, NULL}
  };

  parmac_set(p,"tvar",pos,&state_start,&state_end,trsns,endof(trsns));
}

void tcl_parser_sstr_machine(struct parmac *p,PARMAC_POS pos) {
  static const struct parmac_state
    state_start={"start",NULL,NULL},
    state_sstr={"sstr",tcl_parser_on_str,NULL},
    state_var={"var",NULL,NULL},
    state_cmd={"cmd",NULL,NULL},
    state_end={"end",NULL,NULL};

  static const struct parmac_transition trsns[]={
    {&state_start, &state_sstr, tcl_parser_parse_sstr, NULL},
    {&state_start, &state_var, NULL, tcl_parser_var_machine},
    {&state_start, &state_cmd, NULL, tcl_parser_cmd_machine},

    {&state_var, &state_sstr, tcl_parser_parse_sstr, NULL},
    {&state_var, &state_var, NULL, tcl_parser_var_machine},
    {&state_var, &state_cmd, NULL, tcl_parser_cmd_machine},
    {&state_var, &state_end, NULL, NULL},

    {&state_cmd, &state_sstr, tcl_parser_parse_sstr, NULL},
    {&state_cmd, &state_var, NULL, tcl_parser_var_machine},
    {&state_cmd, &state_cmd, NULL, tcl_parser_cmd_machine},
    {&state_cmd, &state_end, NULL, NULL},

    {&state_sstr, &state_var, NULL, tcl_parser_var_machine},
    {&state_sstr, &state_cmd, NULL, tcl_parser_cmd_machine},
    {&state_sstr, &state_end, NULL, NULL}
  };

  parmac_set(p,"tsstr",pos,&state_start,&state_end,trsns,endof(trsns));
}

void tcl_parser_qstr_machine(struct parmac *p,PARMAC_POS pos) {
  static const struct parmac_state
    state_start={"start",NULL,NULL},
    state_lquote={"lquote",NULL,NULL},
    state_rquote={"rquote",NULL,NULL},
    state_qstr={"qstr",tcl_parser_on_str,NULL},
    state_var={"var",NULL,NULL},
    state_cmd={"cmd",NULL,NULL},
    state_end={"end",NULL,NULL};

  static const struct parmac_transition trsns[]={
    {&state_start, &state_lquote, tcl_parser_parse_lquote, NULL},

    {&state_lquote, &state_rquote, tcl_parser_parse_rquote, NULL},
    {&state_lquote, &state_qstr, tcl_parser_parse_qstr, NULL},
    {&state_lquote, &state_var, NULL, tcl_parser_var_machine},
    {&state_lquote, &state_cmd, NULL, tcl_parser_cmd_machine},

    {&state_var, &state_rquote, tcl_parser_parse_rquote, NULL},
    {&state_var, &state_qstr, tcl_parser_parse_qstr, NULL},
    {&state_var, &state_var, NULL, tcl_parser_var_machine},
    {&state_var, &state_cmd, NULL, tcl_parser_cmd_machine},

    {&state_cmd, &state_rquote, tcl_parser_parse_rquote, NULL},
    {&state_cmd, &state_qstr, tcl_parser_parse_qstr, NULL},
    {&state_cmd, &state_var, NULL, tcl_parser_var_machine},
    {&state_cmd, &state_cmd, NULL, tcl_parser_cmd_machine},

    {&state_qstr, &state_rquote, tcl_parser_parse_rquote, NULL},
    {&state_qstr, &state_var, NULL, tcl_parser_var_machine},
    {&state_qstr, &state_cmd, NULL, tcl_parser_cmd_machine},

    {&state_rquote, &state_end, NULL, NULL}
  };

  parmac_set(p,"tqstr",pos,&state_start,&state_end,trsns, endof(trsns));
}

void tcl_parser_bstr_machine(struct parmac *p,PARMAC_POS pos) {
  static const struct parmac_state
    state_start={"start",NULL,NULL},
    state_lbrace={"lbrace",NULL,NULL},
    state_rbrace={"rbrace",NULL,NULL},
    state_bstr={"bstr",tcl_parser_on_bstr,NULL},
    state_end={"end",NULL,NULL};

  static const struct parmac_transition trsns[]={
    {&state_start, &state_lbrace, tcl_parser_parse_lbrace, NULL},

    {&state_lbrace, &state_rbrace, tcl_parser_parse_rbrace, NULL},
    {&state_lbrace, &state_bstr, tcl_parser_parse_bstr, NULL},

    {&state_bstr, &state_rbrace, tcl_parser_parse_rbrace, NULL},

    {&state_rbrace, &state_end, NULL, NULL},
  };

  parmac_set(p,"tbstr",pos,&state_start,&state_end,trsns,endof(trsns));
}

void tcl_parser_word_machine(struct parmac *p,PARMAC_POS pos) {
  static const struct parmac_state
    state_start={"start",NULL,NULL},
    state_bstr={"bstr",NULL,NULL},
    state_qstr={"qstr",NULL,NULL},
    state_sstr={"sstr",NULL,NULL},
    state_end={"end",NULL,NULL};

  static const struct parmac_transition trsns[]={
    {&state_start, &state_qstr, NULL, tcl_parser_qstr_machine},
    {&state_start, &state_bstr, NULL, tcl_parser_bstr_machine},
    {&state_start, &state_sstr, NULL, tcl_parser_sstr_machine},

    {&state_qstr, &state_end, NULL, NULL},
    {&state_bstr, &state_end, NULL, NULL},
    {&state_sstr, &state_end, NULL, NULL}
  };

  parmac_set(p,"tword",pos,&state_start,&state_end,trsns,endof(trsns));
}

void tcl_parser_main_machine(struct parmac *p,PARMAC_POS pos) {
  static const struct parmac_state
    state_start={"start",NULL,NULL},
    state_cmnt={"cmnt",NULL,NULL},
    state_word={"word",tcl_parser_on_word,NULL},
    state_spc={"spc",NULL,NULL},
    state_sep={"sep",tcl_parser_on_stmt,NULL},
    state_eol={"eol",tcl_parser_on_stmt,NULL},
    state_end={"end",tcl_parser_on_stmt,NULL};

  static const struct parmac_transition trsns[]={
    {&state_start, &state_cmnt, tcl_parser_parse_cmnt, NULL},
    {&state_start, &state_sep, tcl_parser_parse_sep,  NULL},
    {&state_start, &state_eol, tcl_parser_parse_eol,  NULL},
    {&state_start, &state_spc, tcl_parser_parse_spc,  NULL},
    {&state_start, &state_word, NULL, tcl_parser_word_machine},
    {&state_start, &state_end, NULL, NULL},

    {&state_cmnt, &state_eol, tcl_parser_parse_eol, NULL},
    {&state_cmnt, &state_end, NULL, NULL},

    {&state_word, &state_spc, tcl_parser_parse_spc, NULL},
    {&state_word, &state_sep, tcl_parser_parse_sep, NULL},
    {&state_word, &state_eol, tcl_parser_parse_eol, NULL},
    {&state_word, &state_end, NULL, NULL},

    {&state_spc, &state_sep, tcl_parser_parse_sep, NULL},
    {&state_spc, &state_eol, tcl_parser_parse_eol, NULL},
    {&state_spc, &state_word, NULL, tcl_parser_word_machine},
    {&state_spc, &state_end, NULL, NULL},

    {&state_sep, &state_cmnt, tcl_parser_parse_cmnt, NULL},
    {&state_sep, &state_spc, tcl_parser_parse_spc, NULL},
    {&state_sep, &state_eol, tcl_parser_parse_eol, NULL},
    {&state_sep, &state_word, NULL, tcl_parser_word_machine},
    {&state_sep, &state_end, NULL, NULL},

    {&state_eol, &state_cmnt, tcl_parser_parse_cmnt, NULL},
    {&state_eol, &state_spc, tcl_parser_parse_spc,  NULL},
    {&state_eol, &state_sep, tcl_parser_parse_sep,  NULL},
    {&state_eol, &state_word, NULL, tcl_parser_word_machine},
    {&state_eol, &state_end, NULL, NULL}
  };

  parmac_set(p,"tmain",pos,&state_start,&state_end,trsns, endof(trsns));
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
