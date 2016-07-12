#include "tcl_parser.h"

#include <stdio.h>
#include <stddef.h>

#define parse_sep tcl_parser_parse_sep
#define parse_eol tcl_parser_parse_eol
#define parse_spc tcl_parser_parse_spc
#define parse_cmnt tcl_parser_parse_cmnt
#define parse_lquote tcl_parser_parse_lquote
#define parse_rquote tcl_parser_parse_rquote
#define parse_lsqr tcl_parser_parse_lsqr
#define parse_rsqr tcl_parser_parse_rsqr
#define parse_lbrace tcl_parser_parse_lbrace
#define parse_rbrace tcl_parser_parse_rbrace
#define parse_sstr tcl_parser_parse_sstr
#define parse_qstr tcl_parser_parse_qstr
#define parse_bstr tcl_parser_parse_bstr
#define parse_var_str tcl_parser_parse_var_str
#define parse_var_idn tcl_parser_parse_var_idn
#define parse_dollar tcl_parser_parse_dollar

#define main_machine tcl_parser_main_machine
#define word_machine tcl_parser_word_machine
#define bstr_machine tcl_parser_bstr_machine
#define qstr_machine tcl_parser_qstr_machine
#define sstr_machine tcl_parser_sstr_machine
#define cmd_machine tcl_parser_cmd_machine
#define var_machine tcl_parser_var_machine

struct tcl_syntax *tcl_syntax_push(struct tcl_syntax **pSyntax,
                                   unsigned int *pSyntaxNext,
                                   unsigned int *pSyntaxNum,
                                   unsigned int depth,
                                   enum tcl_syntax_type type) {

  //
  struct tcl_syntax *top;
  top=&((*pSyntax)[(*pSyntaxNext)-1]);

  if(type==tcl_syntax_sep && top->depth==depth && top->type==tcl_syntax_sep) {
    return top;
  }

  if(type==tcl_syntax_sep && top->depth==depth && top->type==tcl_syntax_spc) {
    top->type=tcl_syntax_sep;
    return top;
  }

  //
  if(*pSyntaxNext >= *pSyntaxNum) {
    (*pSyntaxNum)*=2;
    *pSyntax=(struct tcl_syntax*)realloc(*pSyntax,sizeof(struct tcl_syntax)*(*pSyntaxNum));
  }

  //
  struct tcl_syntax *cur=&((*pSyntax)[*pSyntaxNext]);
  cur->type=type;
  cur->depth=depth;
  cur->charsNum=0;
  (*pSyntaxNext)++;

  return cur;
}


char *tcl_syntax_str_push(char **pSyntaxChars,
                           unsigned int *pSyntaxCharsNext,
                           unsigned int *pSyntaxCharsNum,
                           unsigned int len) {
  if((*pSyntaxCharsNext)+len >= (*pSyntaxCharsNum)) {
    while((*pSyntaxCharsNext)+len >= (*pSyntaxCharsNum)) {
      (*pSyntaxCharsNum)*=2;
    }

    *pSyntaxChars=(char*)realloc(*pSyntaxChars,*pSyntaxCharsNum);
  }

  char *r=&((*pSyntaxChars)[*pSyntaxCharsNext]);
  (*pSyntaxCharsNext)+=len;
  return r;
}


void char_enter(unsigned int stkDepth,
                const struct parmac_state *fromState,
                const struct parmac_state *toState,
                const char *srcStart,const char *srcEnd,
                void *data) {


  struct tcl_parser *tp=(struct tcl_parser*)data;

  if(fromState!=toState) {
    tp->markStart=srcStart;
  }

  tp->markEnd=srcEnd;
}


void sstr_leave(unsigned int stkDepth,
                const struct parmac_state *fromState,
                const struct parmac_state *toState,
                void *data) {
  struct tcl_parser *tp=(struct tcl_parser*)data;

  printf("----- %u sstr '%.*s'\n",tp->depth,
         (unsigned int)(tp->markEnd-tp->markStart),
         tp->markStart);


  struct tcl_syntax *n;
  n=tcl_syntax_push(&tp->syntax,&tp->syntaxNext,&tp->syntaxNum,tp->depth,tcl_syntax_str);

  const char *c;
  char *s;
  c=tp->markStart;

  while(c!=tp->markEnd) {
    s=tcl_syntax_str_push(&tp->syntaxChars,
                          &tp->syntaxCharsNext,
                          &tp->syntaxCharsNum,1);
    n->charsNum+=1;

    if(c[0]=='\\' && c[1]=='t') {
      s[0]='\t';
      c+=2;
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


  s=tcl_syntax_str_push(&tp->syntaxChars,
                      &tp->syntaxCharsNext,
                        &tp->syntaxCharsNum,1);
  s[0]='\0';

}

void qstr_leave(unsigned int stkDepth,
                const struct parmac_state *fromState,
                const struct parmac_state *toState,
                void *data) {
  struct tcl_parser *tp=(struct tcl_parser*)data;

  printf("----- %u qstr '%.*s'\n",tp->depth,
         (unsigned int)(tp->markEnd-tp->markStart),
         tp->markStart);


  struct tcl_syntax *n;
  n=tcl_syntax_push(&tp->syntax,&tp->syntaxNext,&tp->syntaxNum,tp->depth,tcl_syntax_str);

  const char *c;
  char *s;
  c=tp->markStart;

  while(c!=tp->markEnd) {
    s=tcl_syntax_str_push(&tp->syntaxChars,
                          &tp->syntaxCharsNext,
                          &tp->syntaxCharsNum,1);
    n->charsNum+=1;

    if(c[0]=='\\' && (c[1]=='\n' || (c[1]=='\r' && c[2]=='\n'))) {
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


  s=tcl_syntax_str_push(&tp->syntaxChars,
                        &tp->syntaxCharsNext,
                        &tp->syntaxCharsNum,1);
  s[0]='\0';

}

void bstr_leave(unsigned int stkDepth,
                const struct parmac_state *fromState,
                const struct parmac_state *toState,
                void *data) {
  struct tcl_parser *tp=(struct tcl_parser*)data;


  printf("----- %u bstr '%.*s'\n",tp->depth,
         (unsigned int)(tp->markEnd-tp->markStart),
         tp->markStart);


  struct tcl_syntax *n;
  n=tcl_syntax_push(&tp->syntax,&tp->syntaxNext,&tp->syntaxNum,tp->depth,tcl_syntax_str);

  char *s;
  const char *c;
  c=tp->markStart;

  while(c!=tp->markEnd) {
    s=tcl_syntax_str_push(&tp->syntaxChars,
                          &tp->syntaxCharsNext,
                          &tp->syntaxCharsNum,1);
    n->charsNum+=1;

    if(c[0]=='\\' && (c[1]=='\n' || (c[1]=='\r' && c[2]=='\n'))) {
      s[0]=' ';
      c+=(c[1]=='\n')?2:3;
    } else {
      s[0]=c[0];
      c++;
    }
  }


  s=tcl_syntax_str_push(&tp->syntaxChars,
                        &tp->syntaxCharsNext,
                        &tp->syntaxCharsNum,1);
  s[0]='\0';

}

void var_leave(unsigned int stkDepth,
               const struct parmac_state *fromState,
               const struct parmac_state *toState,
               void *data) {
  struct tcl_parser *tp=(struct tcl_parser*)data;

  printf("----- %u var '%.*s'\n",tp->depth,(int)(tp->markEnd-tp->markStart),tp->markStart);


  struct tcl_syntax *n;
  char *s;

  //set
  n=tcl_syntax_push(&tp->syntax,&tp->syntaxNext,&tp->syntaxNum,tp->depth+1,tcl_syntax_str);
  n->charsNum=3;
  s=tcl_syntax_str_push(&tp->syntaxChars,
                        &tp->syntaxCharsNext,
                        &tp->syntaxCharsNum,4);
  sprintf(s,"set");

  //
  tcl_syntax_push(&tp->syntax,&tp->syntaxNext,&tp->syntaxNum,tp->depth+1,tcl_syntax_spc);

  //var_name
  n=tcl_syntax_push(&tp->syntax,&tp->syntaxNext,&tp->syntaxNum,tp->depth+1,tcl_syntax_str);
  n->charsNum=(unsigned int)(tp->markEnd-tp->markStart);
  s=tcl_syntax_str_push(&tp->syntaxChars,
                        &tp->syntaxCharsNext,
                        &tp->syntaxCharsNum,
                        n->charsNum+1);
  sprintf(s,"%.*s",n->charsNum,tp->markStart);

  //
  tcl_syntax_push(&tp->syntax,&tp->syntaxNext,&tp->syntaxNum,tp->depth+1,tcl_syntax_sep);

}

void word_leave(unsigned int stkDepth,
                const struct parmac_state *fromState,
                const struct parmac_state *toState,
                void *data) {
  struct tcl_parser *tp=(struct tcl_parser*)data;

  printf("----- %u word\n",tp->depth);

  tcl_syntax_push(&tp->syntax,&tp->syntaxNext,&tp->syntaxNum,tp->depth,tcl_syntax_spc);

}

void stmt_leave(unsigned int stkDepth,
                const struct parmac_state *fromState,
                const struct parmac_state *toState,
                void *data) {
  struct tcl_parser *tp=(struct tcl_parser*)data;

  printf("----- %u stmt\n",tp->depth);

  tcl_syntax_push(&tp->syntax,&tp->syntaxNext,&tp->syntaxNum,tp->depth,tcl_syntax_sep);
}

void cmd_enter(unsigned int stkDepth,
               const struct parmac_state *fromState,
               const struct parmac_state *toState,
               const char *srcStart,const char *srcEnd,
               void *data) {

  struct tcl_parser *tp=(struct tcl_parser*)data;
  tp->depth++;
}

void cmd_leave(unsigned int stkDepth,
               const struct parmac_state *fromState,
               const struct parmac_state *toState,
               void *data) {

  struct tcl_parser *tp=(struct tcl_parser*)data;
  tp->depth--;
}

const char *parse_sep(const char *src,const char **name,void *data) {
  struct tcl_parser *tp=(struct tcl_parser*)data;
  *name="sep";
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

const char *parse_eol(const char *src,const char **name,void *data) {
  struct tcl_parser *tp=(struct tcl_parser*)data;
  *name="eol";
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

const char *parse_spc(const char *src,const char **name,void *data) {
  struct tcl_parser *tp=(struct tcl_parser*)data;
  *name="spc";
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

const char *parse_cmnt(const char *src,const char **name,void *data) {
  struct tcl_parser *tp=(struct tcl_parser*)data;
  *name="cmnt";

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

const char *parse_lquote(const char *src,const char **name,void *data) {
  struct tcl_parser *tp=(struct tcl_parser*)data;
  *name="lquote";

  if(src[0]!='"') {
    return NULL;
  }

  tp->errMsg=NULL;
  tp->closings[++(tp->closingsInd)]='"';
  return src+1;
}

const char *parse_rquote(const char *src,const char **name,void *data) {
  struct tcl_parser *tp=(struct tcl_parser*)data;
  *name="rquote";

  if(src[0]!='"') {
    tp->errMsg="Expecting closing double quote.\n";
    return NULL;
  }

  tp->errMsg=NULL;
  (tp->closingsInd)--;
  return src+1;
}

const char *parse_lsqr(const char *src,const char **name,void *data) {
  struct tcl_parser *tp=(struct tcl_parser*)data;
  *name="lsqr";

  if(src[0]!='[') {
    return NULL;
  }

  tp->errMsg=NULL;
  tp->closings[++(tp->closingsInd)]=']';
  return src+1;
}

const char *parse_rsqr(const char *src,const char **name,void *data) {
  struct tcl_parser *tp=(struct tcl_parser*)data;
  *name="rsqr";

  if(src[0]!=']') {
    tp->errMsg="Expecting closing square bracket.\n";
    return NULL;
  }

  tp->errMsg=NULL;
  (tp->closingsInd)--;
  return src+1;
}

const char *parse_lbrace(const char *src,const char **name,void *data) {
  struct tcl_parser *tp=(struct tcl_parser*)data;
  *name="lbrace";

  if(src[0]!='{') {
    return NULL;
  }

  tp->errMsg=NULL;
  tp->closings[++(tp->closingsInd)]='}';
  return src+1;
}

const char *parse_rbrace(const char *src,const char **name,void *data) {
  struct tcl_parser *tp=(struct tcl_parser*)data;
  *name="rbrace";

  if(src[0]!='}') {
    tp->errMsg="Expecting closing curly brace.\n";
    return NULL;
  }

  tp->errMsg=NULL;
  (tp->closingsInd)--;
  return src+1;
}

const char *parse_sstr(const char *src,const char **name,void *data) {
  struct tcl_parser *tp=(struct tcl_parser*)data;
  *name="sstr";
  const char *start=src;

  while(true) {
    if(src[0]==' ' || src[0]=='\t' || src[0]==';' || src[0]=='\0') {
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

const char *parse_qstr(const char *src,const char **name,void *data) {
  struct tcl_parser *tp=(struct tcl_parser*)data;
  *name="qstr";
  const char *start=src;

  while(true) {
    if(src[0]=='"' || src[0]=='\0') {
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

const char *parse_bstr(const char *src,const char **name,void *data) {
  struct tcl_parser *tp=(struct tcl_parser*)data;
  *name="bstr";
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

const char *parse_var_str(const char *src,const char **name,void *data) {
  struct tcl_parser *tp=(struct tcl_parser*)data;
  *name="var_str";
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

const char *parse_var_idn(const char *src,const char **name,void *data) {
  struct tcl_parser *tp=(struct tcl_parser*)data;
  *name="var_idn";
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

const char *parse_dollar(const char *src,const char **name,void *data) {
  struct tcl_parser *tp=(struct tcl_parser*)data;
  *name="dollar";

  if(src[0]=='$') {
    tp->errMsg=NULL;
    return src+1;
  }

  return NULL;
}

void main_machine(struct parmac *p,const char *src);
void stmt_machine(struct parmac *p,const char *src);
void word_machine(struct parmac *p,const char *src);
void bstr_machine(struct parmac *p,const char *src);
void qstr_machine(struct parmac *p,const char *src);
void sstr_machine(struct parmac *p,const char *src);
void var_machine(struct parmac *p,const char *src);
void cmd_machine(struct parmac *p,const char *src);

void main_machine(struct parmac *p,const char *src) {
  static const struct parmac_state
    state_start={"start",NULL,NULL},
    state_cmnt={"cmnt",NULL,NULL},
    state_word={"word",NULL,word_leave},
    state_spc={"spc",NULL,NULL},
    state_sep={"sep",NULL,stmt_leave},
    state_eol={"eol",NULL,stmt_leave},
    state_end={"end",NULL,stmt_leave};

  static const struct parmac_transition trsns[]={
    {&state_start, &state_cmnt, parse_cmnt, NULL},
    {&state_start, &state_sep,  parse_sep,  NULL},
    {&state_start, &state_eol,  parse_eol,  NULL},
    {&state_start, &state_spc,  parse_spc,  NULL},
    {&state_start, &state_word, NULL,       word_machine},
    {&state_start, &state_end,  NULL,       NULL},

    {&state_cmnt, &state_eol, parse_eol, NULL},
    {&state_cmnt, &state_end, NULL,      NULL},

    {&state_word, &state_spc, parse_spc, NULL},
    {&state_word, &state_sep, parse_sep, NULL},
    {&state_word, &state_eol, parse_eol, NULL},
    {&state_word, &state_end, NULL,      NULL},

    {&state_spc, &state_sep,  parse_sep, NULL},
    {&state_spc, &state_eol,  parse_eol, NULL},
    {&state_spc, &state_word, NULL,      word_machine},
    {&state_spc, &state_end,  NULL,      NULL},

    {&state_sep, &state_cmnt, parse_cmnt, NULL},
    {&state_sep, &state_spc,  parse_spc,  NULL},
    {&state_sep, &state_eol,  parse_eol,  NULL},
    {&state_sep, &state_word, NULL,       word_machine},
    {&state_sep, &state_end,  NULL,       NULL},

    {&state_eol, &state_cmnt, parse_cmnt, NULL},
    {&state_eol, &state_spc,  parse_spc,  NULL},
    {&state_eol, &state_sep,  parse_sep,  NULL},
    {&state_eol, &state_word, NULL,       word_machine},
    {&state_eol, &state_end,  NULL,       NULL}
  };

  parmac_set(p,"main",src,&state_start,&state_end,trsns, endof(trsns));
}

void word_machine(struct parmac *p,const char *src) {
  static const struct parmac_state
    state_start={"start",NULL,NULL},
    state_bstr={"bstr",NULL,NULL},
    state_qstr={"qstr",NULL,NULL},
    state_sstr={"sstr",NULL,NULL},
    state_end={"end",NULL,NULL};

  static const struct parmac_transition trsns[]={
    {&state_start, &state_qstr, NULL, qstr_machine},
    {&state_start, &state_bstr, NULL, bstr_machine},
    {&state_start, &state_sstr, NULL, sstr_machine},

    {&state_qstr, &state_end, NULL, NULL},
    {&state_bstr, &state_end, NULL, NULL},
    {&state_sstr, &state_end, NULL, NULL}
  };

  parmac_set(p,"word",src,&state_start,&state_end,trsns,endof(trsns));
}

void bstr_machine(struct parmac *p,const char *src) {
  static const struct parmac_state
    state_start={"start",NULL,NULL},
    state_lbrace={"lbrace",NULL,NULL},
    state_rbrace={"rbrace",NULL,NULL},
    state_bstr={"bstr",char_enter,bstr_leave},
    state_end={"end",NULL,NULL};

  static const struct parmac_transition trsns[]={
    {&state_start, &state_lbrace, parse_lbrace, NULL},

    {&state_lbrace, &state_rbrace, parse_rbrace, NULL},
    {&state_lbrace, &state_bstr,    parse_bstr,   NULL},

    {&state_bstr, &state_rbrace, parse_rbrace, NULL},

    {&state_rbrace, &state_end, NULL, NULL},
  };

  parmac_set(p,"bstr",src,&state_start,&state_end,trsns,endof(trsns));
}

void qstr_machine(struct parmac *p,const char *src) {
  static const struct parmac_state
    state_start={"start",NULL,NULL},
    state_lquote={"lquote",NULL,NULL},
    state_rquote={"rquote",NULL,NULL},
    state_qstr={"qstr",char_enter,qstr_leave},
    state_var={"var",char_enter,NULL},
    state_cmd={"cmd",NULL,NULL},
    state_end={"end",NULL,NULL};

  static const struct parmac_transition trsns[]={
    {&state_start, &state_lquote, parse_lquote, NULL},

    {&state_lquote, &state_rquote, parse_rquote, NULL},
    {&state_lquote, &state_qstr,   parse_qstr,  NULL},
    {&state_lquote, &state_var,    NULL,        var_machine},
    {&state_lquote, &state_cmd,    NULL,        cmd_machine},

    {&state_var, &state_rquote, parse_rquote, NULL},
    {&state_var, &state_qstr,   parse_qstr,   NULL},
    {&state_var, &state_var,    NULL,         var_machine},
    {&state_var, &state_cmd,    NULL,         cmd_machine},

    {&state_cmd, &state_rquote, parse_rquote, NULL},
    {&state_cmd, &state_qstr,   parse_qstr,   NULL},
    {&state_cmd, &state_var,    NULL,         var_machine},
    {&state_cmd, &state_cmd,    NULL,         cmd_machine},

    {&state_qstr, &state_rquote, parse_rquote, NULL},
    {&state_qstr, &state_var,    NULL,         var_machine},
    {&state_qstr, &state_cmd,    NULL,         cmd_machine},

    {&state_rquote, &state_end, NULL, NULL}
  };

  parmac_set(p,"qstr",src,&state_start,&state_end,trsns, endof(trsns));
}

void sstr_machine(struct parmac *p,const char *src) {
  static const struct parmac_state
    state_start={"start",NULL,NULL},
    state_sstr={"sstr",char_enter,sstr_leave},
    state_var={"var",char_enter,NULL},
    state_cmd={"cmd",NULL,NULL},
    state_end={"end",NULL,NULL};

  static const struct parmac_transition trsns[]={
    {&state_start, &state_sstr, parse_sstr, NULL},
    {&state_start, &state_var,  NULL,       var_machine},
    {&state_start, &state_cmd,  NULL,       cmd_machine},

    {&state_var, &state_sstr, parse_sstr, NULL},
    {&state_var, &state_var,  NULL,       var_machine},
    {&state_var, &state_cmd,  NULL,       cmd_machine},
    {&state_var, &state_end,  NULL,       NULL},

    {&state_cmd, &state_sstr, parse_sstr, NULL},
    {&state_cmd, &state_var,  NULL,       var_machine},
    {&state_cmd, &state_cmd,  NULL,       cmd_machine},
    {&state_cmd, &state_end,  NULL,       NULL},

    {&state_sstr, &state_var, NULL, var_machine},
    {&state_sstr, &state_cmd, NULL, cmd_machine},
    {&state_sstr, &state_end, NULL, NULL}
  };

  parmac_set(p,"sstr",src,&state_start,&state_end,trsns,endof(trsns));
}

void var_machine(struct parmac *p,const char *src) {
  static const struct parmac_state
    state_start={"start",NULL,NULL},
    state_dollar={"dollar",NULL,NULL},
    state_idn={"idn",char_enter,NULL},
    state_str={"str",char_enter,NULL},
    state_lbrace={"lbrace",NULL,NULL},
    state_rbrace={"rbrace",NULL,NULL},
    state_end={"end",NULL,var_leave};

  static const struct parmac_transition trsns[]={
    {&state_start, &state_dollar, parse_dollar, NULL},

    {&state_dollar, &state_lbrace, parse_lbrace,  NULL},
    {&state_dollar, &state_idn,    parse_var_idn, NULL},

    {&state_lbrace, &state_rbrace, parse_rbrace,  NULL},
    {&state_lbrace, &state_str,    parse_var_str, NULL},

    {&state_str, &state_rbrace, parse_rbrace, NULL},

    {&state_rbrace, &state_end, NULL, NULL},

    {&state_idn, &state_end, NULL, NULL}
  };

  parmac_set(p,"vstr",src,&state_start,&state_end,trsns,endof(trsns));
}

void cmd_machine(struct parmac *p,const char *src) {
  static const struct parmac_state
    state_start={"start",cmd_enter,NULL},
    state_lsqr={"lsqr",NULL,NULL},
    state_rsqr={"rsqr",NULL,NULL},
    state_main={"main",NULL,NULL},
    state_end={"end",NULL,cmd_leave};

  static const struct parmac_transition trsns[]={
    {&state_start, &state_lsqr, parse_lsqr, NULL},

    {&state_lsqr, &state_rsqr, parse_rsqr, NULL},
    {&state_lsqr, &state_main, NULL,       main_machine},

    {&state_main, &state_rsqr, parse_rsqr, NULL},

    {&state_rsqr, &state_end, NULL, NULL}
  };

  parmac_set(p,"cmd",src,&state_start,&state_end,trsns, endof(trsns));
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

void tcl_parser_run(struct tcl_parser *tp,const char *src) {
  unsigned int stkDepth=0;
  bool err;

  tp->errMsg=NULL;



  tp->closingsInd=0;

  tp->markStart=src;
  tp->markEnd=src;

  tp->pos=0;
  tp->row=0;
  tp->col=0;

  tp->depth=0;

  tp->syntaxNum=2;
  tp->syntaxNext=0;
  tp->syntax=(struct tcl_syntax*)malloc(sizeof(struct tcl_syntax)*tp->syntaxNum);

  tp->syntaxCharsNum=16;
  tp->syntaxCharsNext=0;
  tp->syntaxChars=(char*)malloc(tp->syntaxCharsNum);

  tcl_parser_main_machine(tp->stk,src);

  while(parmac_run(tp->stk,&stkDepth,tp,&err)) {
    if(stkDepth+1==tp->stkNum) {
      tp->stkNum*=2;
      tp->stk=(struct parmac*)realloc(tp->stk,sizeof(struct parmac)*tp->stkNum);
      tp->closings=(char*)realloc(tp->closings,tp->stkNum);
    }
  }

  printf("\n");

  unsigned int i,c=0;

  for(i=0;i<tp->syntaxNext;i++) {
    struct tcl_syntax *cur=&tp->syntax[i];
    printf("%u : ",cur->depth);

    if(cur->type==tcl_syntax_str) {
      printf("str '%s'",&tp->syntaxChars[c]);
      c+=cur->charsNum+1;
    } else if(cur->type==tcl_syntax_spc) {
      printf("spc");
    } else if(cur->type==tcl_syntax_sep) {
      printf("sep");
    }

    printf("\n");
  }

  printf("\n");

  if(err) {
    printf("Error.\n");

    if(tp->errMsg!=NULL) {
      printf(tp->errMsg);
    }
  } else if(tp->stk[0].src[0]!='\0') {
    printf("Error.\nUnexpected '%s'\n",tp->stk[0].src);
  }

  //
  // printSyntax(tp.rootStmt,0);



  printf("done %i.\n", stkDepth);

}
