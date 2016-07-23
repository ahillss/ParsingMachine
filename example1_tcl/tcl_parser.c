#include "tcl_parser.h"

#include <stdio.h>
#include <stddef.h>

// #define TCL_PARSER_DEBUG

#ifdef TCL_PARSER_DEBUG
#define TCL_PARSER_DEBUG_PRINTF(...) printf(__VA_ARGS__)
#else
#define TCL_PARSER_DEBUG_PRINTF(...)
#endif

#define endof(x) (x+sizeof(x)/sizeof(*x))

void tcl_parser_str_leave(unsigned int stkDepth,
                          const char *machine,
                          const char *fromState,
                          const char *toState,
                          const char *srcStart,
                          const char *srcEnd,
                          void *data) {
  struct tcl_parser *tp=(struct tcl_parser*)data;

  TCL_PARSER_DEBUG_PRINTF("%u str '%.*s'\n",tp->depth,(unsigned int)(srcEnd-srcStart),srcStart);

  struct tcl_syntax_node *n;
  n=tcl_syntax_push(tp->syntax,tp->depth,
                    (unsigned int)(srcStart-tp->src),
                    tcl_syntax_str);

  const char *c;
  char *s;
  c=srcStart;

  while(c!=srcEnd) {
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

void tcl_parser_bstr_leave(unsigned int stkDepth,
                           const char *machine,
                           const char *fromState,
                           const char *toState,
                           const char *srcStart,
                           const char *srcEnd,
                           void *data) {
  struct tcl_parser *tp=(struct tcl_parser*)data;


  TCL_PARSER_DEBUG_PRINTF("%u bstr '%.*s'\n",tp->depth,(unsigned int)(srcEnd-srcStart),srcStart);


  struct tcl_syntax_node *n;
  n=tcl_syntax_push(tp->syntax,tp->depth,
                    (unsigned int)(srcStart-tp->src),
                    tcl_syntax_str);

  char *s;
  const char *c;
  c=srcStart;

  while(c!=srcEnd) {
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

void tcl_parser_var_leave(unsigned int stkDepth,
                          const char *machine,
                          const char *fromState,
                          const char *toState,
                          const char *srcStart,
                          const char *srcEnd,
                          void *data) {
  struct tcl_parser *tp=(struct tcl_parser*)data;

  TCL_PARSER_DEBUG_PRINTF("%u var '%.*s'\n",tp->depth,(int)(srcEnd-srcStart),srcStart);


  struct tcl_syntax_node *n;
  char *s;

  //set
  n=tcl_syntax_push(tp->syntax,tp->depth+1,
                    (unsigned int)(srcStart-tp->src),
                    tcl_syntax_str);
  n->charsNum=3;
  s=tcl_syntax_str_push(tp->syntax,4);
  sprintf(s,"set");

  //
  tcl_syntax_push(tp->syntax,tp->depth+1,
                  (unsigned int)(srcStart-tp->src),
                  tcl_syntax_spc);

  //var_name
  n=tcl_syntax_push(tp->syntax,tp->depth+1,
                    (unsigned int)(srcStart-tp->src),
                    tcl_syntax_str);
  n->charsNum=(unsigned int)(srcEnd-srcStart);
  s=tcl_syntax_str_push(tp->syntax,
                        n->charsNum+1);
  sprintf(s,"%.*s",n->charsNum,srcStart);

  //
  tcl_syntax_push(tp->syntax,tp->depth+1,
                  (unsigned int)(srcStart-tp->src),
                  tcl_syntax_sep);

}

void tcl_parser_word_leave(unsigned int stkDepth,
                           const char *machine,
                           const char *fromState,
                           const char *toState,
                           const char *srcStart,
                           const char *srcEnd,
                void *data) {
  struct tcl_parser *tp=(struct tcl_parser*)data;

  TCL_PARSER_DEBUG_PRINTF("%u word\n",tp->depth);

  tcl_syntax_push(tp->syntax,tp->depth,
                  (unsigned int)(srcStart-tp->src),
                  tcl_syntax_spc);

}

void tcl_parser_stmt_leave(unsigned int stkDepth,
                           const char *machine,
                           const char *fromState,
                           const char *toState,
                           const char *srcStart,
                           const char *srcEnd,
                           void *data) {
  struct tcl_parser *tp=(struct tcl_parser*)data;

  TCL_PARSER_DEBUG_PRINTF("%u stmt\n",tp->depth);

  tcl_syntax_push(tp->syntax,tp->depth,
                  (unsigned int)(srcStart-tp->src),
                  tcl_syntax_sep);
}

void tcl_parser_cmd_enter(unsigned int stkDepth,
                          const char *machine,
                          const char *fromState,
                          const char *toState,
                          const char *srcStart,
                          const char *srcEnd,
                          void *data) {

  struct tcl_parser *tp=(struct tcl_parser*)data;
  tp->depth++;
}

void cmd_leave(unsigned int stkDepth,
               const char *machine,
               const char *fromState,
               const char *toState,
               const char *srcStart,
               const char *srcEnd,
               void *data) {

  struct tcl_parser *tp=(struct tcl_parser*)data;
  tp->depth--;
}

const char *tcl_parser_parse_sep(const char *src,void *data) {
  struct tcl_parser *tp=(struct tcl_parser*)data;
  const char *start=src;

  while(src[0]==';') {
    src++;
  }

  if(start==src) {
    return NULL;
  }

  tp->errMsg=NULL;
  return src;
}

const char *tcl_parser_parse_eol(const char *src,void *data) {
  struct tcl_parser *tp=(struct tcl_parser*)data;
  const char *start=src;

  while(true) {
    if(src[0]=='\r' && src[1]=='\n') {
      src+=2;
    } else if(src[0]=='\n') {
      src+=1;
    } else {
      break;
    }
  }

  if(start==src) {
    return NULL;
  }

  tp->errMsg=NULL;
  return src;
}

const char *tcl_parser_parse_spc(const char *src,void *data) {
  struct tcl_parser *tp=(struct tcl_parser*)data;
  const char *start=src;

  while(true) {
    if(src[0]==' ' || src[0]=='\t') {
      src+=1;
    } else if(src[0]=='\\' && src[1]=='\n') {
      src+=2;
    } else if(src[0]=='\\' && src[1]=='\r' && src[2]=='\n') {
      src+=3;
    } else {
      break;
    }
  }

  if(start==src) {
    return NULL;
  }

  tp->errMsg=NULL;
  return src;
}

const char *tcl_parser_parse_cmnt(const char *src,void *data) {
  struct tcl_parser *tp=(struct tcl_parser*)data;

  while(src[0]==' ' || src[0]=='\t') {
    src++;
  }

  if(src[0] != '#') {
    return NULL;
  }

  src++;

  while((src[0] != '\r' && src[1] != '\n') &&
        src[0] != '\n' &&
        src[0] != '\0') {
    src++;
  }

  tp->errMsg=NULL;
  return src;
}

const char *tcl_parser_parse_lquote(const char *src,void *data) {
  struct tcl_parser *tp=(struct tcl_parser*)data;

  if(src[0]!='"') {
    return NULL;
  }

  tp->errMsg=NULL;
  tp->closings[++(tp->closingsInd)]='"';
  return src+1;
}

const char *tcl_parser_parse_rquote(const char *src,void *data) {
  struct tcl_parser *tp=(struct tcl_parser*)data;

  if(src[0]!='"') {
    tp->errMsg="Expecting closing double quote.\n";
    return NULL;
  }

  tp->errMsg=NULL;
  (tp->closingsInd)--;
  return src+1;
}

const char *tcl_parser_parse_lsqr(const char *src,void *data) {
  struct tcl_parser *tp=(struct tcl_parser*)data;

  if(src[0]!='[') {
    return NULL;
  }

  tp->errMsg=NULL;
  tp->closings[++(tp->closingsInd)]=']';
  return src+1;
}

const char *tcl_parser_parse_rsqr(const char *src,void *data) {
  struct tcl_parser *tp=(struct tcl_parser*)data;

  if(src[0]!=']') {
    tp->errMsg="Expecting closing square bracket.\n";
    return NULL;
  }

  tp->errMsg=NULL;
  (tp->closingsInd)--;
  return src+1;
}

const char *tcl_parser_parse_lbrace(const char *src,void *data) {
  struct tcl_parser *tp=(struct tcl_parser*)data;

  if(src[0]!='{') {
    return NULL;
  }

  tp->errMsg=NULL;
  tp->closings[++(tp->closingsInd)]='}';
  return src+1;
}

const char *tcl_parser_parse_rbrace(const char *src,void *data) {
  struct tcl_parser *tp=(struct tcl_parser*)data;

  if(src[0]!='}') {
    tp->errMsg="Expecting closing curly brace.\n";
    return NULL;
  }

  tp->errMsg=NULL;
  (tp->closingsInd)--;
  return src+1;
}

const char *tcl_parser_parse_sstr(const char *src,void *data) {
  struct tcl_parser *tp=(struct tcl_parser*)data;
  const char *start=src;

  while(true) {
    if(src[0]==' ' || src[0]=='\t' || src[0]==';' ||
       src[0]=='[' || src[0]=='\0') {
      break;
    }

    if(src[0]=='\n' || (src[0]=='\r' && src[1]=='\n')) {
      break;
    }

    if(src[0]=='\\' && src[1]=='\r' && src[2]=='\n') {
      break;
    }

    if(src[0]=='\\' && src[1]=='\n') {
      break;
    }

    if(tp->closings[tp->closingsInd]==src[0]) {
      break;
    }

    if(src[0]=='$' &&
       (src[1]=='{' || src[1]=='_' ||
        (src[1]>='a' && src[1]<='z') ||
        (src[1]>='A' && src[1]<='Z') ||
        (src[1]>='0' && src[1]<='9'))) {

      break;
    }

    if(src[0]=='\\' && src[1]!='\0') {
      src+=2;
    } else {
      src++;
    }
  }

  if(start==src) {
    return NULL;
  }

  tp->errMsg=NULL;
  return src;
}

const char *tcl_parser_parse_qstr(const char *src,void *data) {
  struct tcl_parser *tp=(struct tcl_parser*)data;
  const char *start=src;

  while(true) {
    if(src[0]=='"' || src[0]=='[' || src[0]=='\0') {
      break;
    }

    if(src[0]=='$' &&
       (src[1]=='{' || src[1]=='_' ||
        (src[1]>='a' && src[1]<='z') ||
        (src[1]>='A' && src[1]<='Z') ||
        (src[1]>='0' && src[1]<='9'))) {

      break;
    }

    if(src[0]=='\\' && src[1]=='"') {
      src+=2;
    } else {
      src++;
    }
  }

  if(start==src) {
    return NULL;
  }

  tp->errMsg=NULL;
  return src;
}

const char *tcl_parser_parse_bstr(const char *src,void *data) {
  struct tcl_parser *tp=(struct tcl_parser*)data;
  const char *start=src;

  unsigned int c=0;

  while(true) {
    if(c==0 && src[0]=='}') {
      break;
    }

    if(src[0]=='\0') {
      break;
    }

    if(src[0]=='{') {
      c++;
    } else if(src[0]=='}') {
      c--;
    }

    if(src[0]=='\\' && src[1]=='}') {
      src+=2;
    } else {
      src++;
    }
  }

  if(start==src) {
    return NULL;
  }

  tp->errMsg=NULL;
  return src;
}

const char *tcl_parser_parse_var_str(const char *src,void *data) {
  struct tcl_parser *tp=(struct tcl_parser*)data;
  const char *start=src;

  unsigned int c=0;


  while(true) {
    if(c==0 && src[0]=='}') {
      break;
    }

    if(src[0]=='\0') {
      break;
    }

    if(src[0]=='{') {
      c++;
    } else if(src[0]=='}') {
      c--;
    }

    src++;
  }

  if(start==src) {
    return NULL;
  }

  //
  tp->errMsg=NULL;
  return src;
}

const char *tcl_parser_parse_var_idn(const char *src,void *data) {
  struct tcl_parser *tp=(struct tcl_parser*)data;
  const char *start=src;

  while(src[0]=='_' ||
        (src[0]>='a' && src[0]<='z') ||
        (src[0]>='A' && src[0]<='Z') ||
        (src[0]>='0' && src[0]<='9')) {
    src++;
  }

  if(src!=start) {
    tp->errMsg=NULL;
    return src;
  }

  return NULL;
}

const char *tcl_parser_parse_dollar(const char *src,void *data) {
  struct tcl_parser *tp=(struct tcl_parser*)data;

  if(src[0]=='$') {
    tp->errMsg=NULL;
    return src+1;
  }

  return NULL;
}

void tcl_parser_main_machine(struct parmac *p);
void tcl_parser_word_machine(struct parmac *p);
void tcl_parser_bstr_machine(struct parmac *p);
void tcl_parser_qstr_machine(struct parmac *p);
void tcl_parser_sstr_machine(struct parmac *p);
void tcl_parser_var_machine(struct parmac *p);
void tcl_parser_cmd_machine(struct parmac *p);

void tcl_parser_main_machine(struct parmac *p) {
  static const struct parmac_state
    state_start={"start",NULL,NULL},
    state_cmnt={"cmnt",NULL,NULL},
    state_word={"word",NULL,tcl_parser_word_leave},
    state_spc={"spc",NULL,NULL},
    state_sep={"sep",NULL,tcl_parser_stmt_leave},
    state_eol={"eol",NULL,tcl_parser_stmt_leave},
    state_end={"end",NULL,tcl_parser_stmt_leave};

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

  parmac_set(p,"main",&state_start,&state_end,trsns, endof(trsns));
}

void tcl_parser_word_machine(struct parmac *p) {
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

  parmac_set(p,"word",&state_start,&state_end,trsns,endof(trsns));
}

void tcl_parser_bstr_machine(struct parmac *p) {
  static const struct parmac_state
    state_start={"start",NULL,NULL},
    state_lbrace={"lbrace",NULL,NULL},
    state_rbrace={"rbrace",NULL,NULL},
    state_bstr={"bstr",NULL,tcl_parser_bstr_leave},
    state_end={"end",NULL,NULL};

  static const struct parmac_transition trsns[]={
    {&state_start, &state_lbrace, tcl_parser_parse_lbrace, NULL},

    {&state_lbrace, &state_rbrace, tcl_parser_parse_rbrace, NULL},
    {&state_lbrace, &state_bstr, tcl_parser_parse_bstr, NULL},

    {&state_bstr, &state_rbrace, tcl_parser_parse_rbrace, NULL},

    {&state_rbrace, &state_end, NULL, NULL},
  };

  parmac_set(p,"bstr",&state_start,&state_end,trsns,endof(trsns));
}

void tcl_parser_qstr_machine(struct parmac *p) {
  static const struct parmac_state
    state_start={"start",NULL,NULL},
    state_lquote={"lquote",NULL,NULL},
    state_rquote={"rquote",NULL,NULL},
    state_qstr={"qstr",NULL,tcl_parser_str_leave},
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

  parmac_set(p,"qstr",&state_start,&state_end,trsns, endof(trsns));
}

void tcl_parser_sstr_machine(struct parmac *p) {
  static const struct parmac_state
    state_start={"start",NULL,NULL},
    state_sstr={"sstr",NULL,tcl_parser_str_leave},
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

  parmac_set(p,"sstr",&state_start,&state_end,trsns,endof(trsns));
}

void tcl_parser_var_machine(struct parmac *p) {
  static const struct parmac_state
    state_start={"start",NULL,NULL},
    state_dollar={"dollar",NULL,NULL},
    state_idn={"idn",NULL,tcl_parser_var_leave},
    state_str={"str",NULL,tcl_parser_var_leave},
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

  parmac_set(p,"var",&state_start,&state_end,trsns,endof(trsns));
}

void tcl_parser_cmd_machine(struct parmac *p) {
  static const struct parmac_state
    state_start={"start",tcl_parser_cmd_enter,NULL},
    state_lsqr={"lsqr",NULL,NULL},
    state_rsqr={"rsqr",NULL,NULL},
    state_main={"main",NULL,NULL},
    state_end={"end",NULL,cmd_leave};

  static const struct parmac_transition trsns[]={
    {&state_start, &state_lsqr, tcl_parser_parse_lsqr, NULL},

    {&state_lsqr, &state_rsqr, tcl_parser_parse_rsqr, NULL},
    {&state_lsqr, &state_main, NULL, tcl_parser_main_machine},

    {&state_main, &state_rsqr, tcl_parser_parse_rsqr, NULL},

    {&state_rsqr, &state_end, NULL, NULL}
  };

  parmac_set(p,"cmd",&state_start,&state_end,trsns, endof(trsns));
}

void tcl_parser_init(struct tcl_parser *tp) {

  tp->stkNum=2;
  tp->stk=(struct parmac*)malloc(sizeof(struct parmac)*tp->stkNum);

  tp->closings=(char*)malloc(tp->stkNum);
  tp->closings[0]='\0';

}

void tcl_parser_uninit(struct tcl_parser *tp) {
  free(tp->stk);
  free(tp->closings);
}

void tcl_parser_run(struct tcl_parser *tp,struct tcl_syntax *syntax,const char *src) {
  unsigned int stkDepth=0;
  enum parmac_status status;

  tp->closingsInd=0;
  tp->depth=0;
  tp->errMsg=NULL;
  tp->src=src;
  tp->syntax=syntax;

  tcl_parser_main_machine(tp->stk);

  while((status=parmac_run(tp->stk,&stkDepth,src,tp))==parmac_ok) {
    if(stkDepth+1==tp->stkNum) {
      tp->stkNum*=2;
      tp->stk=(struct parmac*)realloc(tp->stk,sizeof(struct parmac)*tp->stkNum);
      tp->closings=(char*)realloc(tp->closings,tp->stkNum);
    }
  }


  unsigned int i,c=0;

  for(i=0;i<tp->syntax->nodesNext;i++) {
    struct tcl_syntax_node *cur=&tp->syntax->nodes[i];


    if(cur->type==tcl_syntax_str) {

      c+=cur->charsNum+1;
    } else if(cur->type==tcl_syntax_spc) {

    } else if(cur->type==tcl_syntax_sep) {

    }


  }


  if(status==parmac_error) {
    printf("Error.\n");

    if(tp->errMsg!=NULL) {
      printf(tp->errMsg);
    }
  } else if(src[tp->stk[stkDepth].pos]!='\0') {
    printf("Error.\nUnexpected '%s'\n",src[tp->stk[stkDepth].pos]);
  }

}
