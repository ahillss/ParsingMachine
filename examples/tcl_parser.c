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
#define parse_schar tcl_parser_parse_schar
#define parse_dollar tcl_parser_parse_dollar
#define parse_idn tcl_parser_parse_idn

#define main_machine tcl_parser_main_machine
#define word_machine tcl_parser_word_machine
#define bstr_machine tcl_parser_bstr_machine
#define qstr_machine tcl_parser_qstr_machine
#define sstr_machine tcl_parser_sstr_machine
#define cmd_machine tcl_parser_cmd_machine
#define vstr_machine tcl_parser_vstr_machine

void printDepth(int d) {
  if(d>0) {
    printf("-");
    printDepth(d-1);
  }
}

struct tcl_syntax *tcl_syntax_push(struct tcl_syntax **pSyntax,
                     unsigned int *pSyntaxNext,
                     unsigned int *pSyntaxNum,
                     enum tcl_syntax_type type,
                     unsigned int depth) {
  if(*pSyntaxNext >= *pSyntaxNum) {
    (*pSyntaxNum)*=2;
    *pSyntax=(struct tcl_syntax*)realloc(*pSyntax,sizeof(struct tcl_syntax)*(*pSyntaxNum));
  }

  struct tcl_syntax *cur=&((*pSyntax)[*pSyntaxNext]);
  cur->type=type;
  cur->depth=depth;
  cur->str=NULL;

  (*pSyntaxNext)++;

  return cur;
}


void tcl_syntax_push_str(struct tcl_syntax **pSyntax,
                         unsigned int *pSyntaxNext,
                         unsigned int *pSyntaxNum,
                         unsigned int depth,
                         const char *strStart,
                         const char *strEnd) {

  struct tcl_syntax *top;
  top=((*pSyntaxNext)!=0)?(&((*pSyntax)[(*pSyntaxNext)-1])):NULL;

  if(top && top->depth==depth && top->type==tcl_syntax_str) {
    unsigned int len1=(unsigned int)strlen(top->str);
    unsigned int len2=(unsigned int)(strEnd-strStart);

    top->str=realloc(top->str,len1+len2+1);
    sprintf(top->str+len1,"%.*s",len2,strStart);
  } else {
    struct tcl_syntax *cur;
    cur=tcl_syntax_push(pSyntax,pSyntaxNext,pSyntaxNum,tcl_syntax_str,depth);

    unsigned int len=(unsigned int)(strEnd-strStart);
    cur->str=(char*)malloc(len+1);
    sprintf(cur->str,"%.*s",len,strStart);
  }
}

void tcl_syntax_push_spc(struct tcl_syntax **pSyntax,
                         unsigned int *pSyntaxNext,
                         unsigned int *pSyntaxNum,
                         unsigned int depth) {

  if(*pSyntaxNext==0) {
    return;
  }

  struct tcl_syntax *cur;

  cur=tcl_syntax_push(pSyntax,pSyntaxNext,pSyntaxNum,tcl_syntax_str,depth);
  cur->type=tcl_syntax_spc;
}


void tcl_syntax_push_sep(struct tcl_syntax **pSyntax,
                         unsigned int *pSyntaxNext,
                         unsigned int *pSyntaxNum,
                         unsigned int depth) {

  if(*pSyntaxNext==0) {
    return;
  }

  struct tcl_syntax *top,*cur;
  top=&((*pSyntax)[(*pSyntaxNext)-1]);

  if(top->depth==depth && top->type==tcl_syntax_sep) {
    return;
  }

  if(top->depth==depth && top->type==tcl_syntax_spc) {
    cur=top;
  } else {
    cur=tcl_syntax_push(pSyntax,pSyntaxNext,pSyntaxNum,tcl_syntax_str,depth);
  }

  cur->type=tcl_syntax_sep;
}

void tcl_syntax_push_var(struct tcl_syntax **pSyntax,
                         unsigned int *pSyntaxNext,
                         unsigned int *pSyntaxNum,
                         unsigned int depth,
                         const char *strStart,
                         const char *strEnd) {
  const char *a="set";
  tcl_syntax_push_str(pSyntax,pSyntaxNext,pSyntaxNum,depth+1,a,a+3);
  tcl_syntax_push_spc(pSyntax,pSyntaxNext,pSyntaxNum,depth+1);
  tcl_syntax_push_str(pSyntax,pSyntaxNext,pSyntaxNum,depth+1,strStart,strEnd);
  tcl_syntax_push_sep(pSyntax,pSyntaxNext,pSyntaxNum,depth+1);
}

////////////////////////////////////

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

void sub_str_leave(unsigned int stkDepth,
                   const struct parmac_state *fromState,
                   const struct parmac_state *toState,
                   void *data) {
  struct tcl_parser *tp=(struct tcl_parser*)data;

  if(fromState!=toState) {
    printf("----- %u sub_str '%.*s'\n",tp->depth,(int)(tp->markEnd-tp->markStart),tp->markStart);

    tcl_syntax_push_str(&tp->syntax,&tp->syntaxNext,&tp->syntaxNum,
                        tp->depth,tp->markStart,tp->markEnd);
  }
}

void bstr_enter(unsigned int stkDepth,
                const struct parmac_state *fromState,
                const struct parmac_state *toState,
                const char *srcStart,const char *srcEnd,
                void *data) {


  struct tcl_parser *tp=(struct tcl_parser*)data;

  tp->markStart=srcStart+1;
  tp->markEnd=srcEnd-1;
}



void var_leave(unsigned int stkDepth,
               const struct parmac_state *fromState,
               const struct parmac_state *toState,
               void *data) {
  struct tcl_parser *tp=(struct tcl_parser*)data;

  printf("----- %u var '%.*s'\n",tp->depth,(int)(tp->markEnd-tp->markStart),tp->markStart);

  tcl_syntax_push_var(&tp->syntax,&tp->syntaxNext,&tp->syntaxNum,
                      tp->depth,tp->markStart,tp->markEnd);
}

void word_leave(unsigned int stkDepth,
                const struct parmac_state *fromState,
                const struct parmac_state *toState,
                void *data) {
  struct tcl_parser *tp=(struct tcl_parser*)data;

  printf("----- %u word\n",tp->depth);

  tcl_syntax_push_spc(&tp->syntax,&tp->syntaxNext,&tp->syntaxNum,tp->depth);
}

void stmt_leave(unsigned int stkDepth,
                const struct parmac_state *fromState,
                const struct parmac_state *toState,
                void *data) {
  struct tcl_parser *tp=(struct tcl_parser*)data;

  printf("----- %u stmt\n",tp->depth);

  tcl_syntax_push_sep(&tp->syntax,&tp->syntaxNext,&tp->syntaxNum,tp->depth);
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

  if(src[0]==';') {
    tp->errMsg=NULL;
    return src+1;
  }

  return NULL;
}

const char *parse_eol(const char *src,const char **name,void *data) {
  struct tcl_parser *tp=(struct tcl_parser*)data;
  *name="eol";

  if(src[0]=='\r' && src[1]=='\n') {
    tp->errMsg=NULL;
    return src+2;
  }

  if(src[0]=='\n') {
    tp->errMsg=NULL;
    return src+1;
  }

  return NULL;
}

const char *parse_spc(const char *src,const char **name,void *data) {
  struct tcl_parser *tp=(struct tcl_parser*)data;
  *name="spc";

  if(src[0]==' ' || src[0]=='\t') {
    tp->errMsg=NULL;
    return src+1;
  }

  return NULL;
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

  if(src[0]=='"') {
    tp->errMsg=NULL;
    tp->closings[++(tp->closingsInd)]='"';
    return src+1;
  }

  return NULL;
}


const char *parse_rquote(const char *src,const char **name,void *data) {
  struct tcl_parser *tp=(struct tcl_parser*)data;
  *name="rquote";

  if(src[0]=='"') {
    (tp->closingsInd)--;
    return src+1;
  }

  tp->errMsg="Expecting closing double quote.\n";
  return NULL;
}

const char *parse_lsqr(const char *src,const char **name,void *data) {
  struct tcl_parser *tp=(struct tcl_parser*)data;
  *name="lsqr";

  if(src[0]=='[') {
    tp->errMsg=NULL;
    tp->closings[++(tp->closingsInd)]=']';
    return src+1;
  }

  return NULL;
}

const char *parse_rsqr(const char *src,const char **name,void *data) {
  struct tcl_parser *tp=(struct tcl_parser*)data;
  *name="rsqr";

  if(src[0]==']') {
    (tp->closingsInd)--;
    return src+1;
  }

  tp->errMsg="Expecting closing square bracket.\n";
  return NULL;
}

const char *parse_lbrace(const char *src,const char **name,void *data) {
  struct tcl_parser *tp=(struct tcl_parser*)data;
  *name="lbrace";

  if(src[0]=='{') {
    tp->errMsg=NULL;
    tp->closings[++(tp->closingsInd)]='}';
    return src+1;
  }

  return NULL;
}

const char *parse_rbrace(const char *src,const char **name,void *data) {
  struct tcl_parser *tp=(struct tcl_parser*)data;
  *name="rbrace";

  if(src[0]=='}') {
    (tp->closingsInd)--;
    return src+1;
  }

  tp->errMsg="Expecting closing curly brace.\n";
  return NULL;
}

const char *parse_schar(const char *src,const char **name,void *data) {
  struct tcl_parser *tp=(struct tcl_parser*)data;
  *name="schar";

  if(src[0]==' ' || src[0]=='\t' || src[0]==';' || src[0]=='\0' ||
     src[0]=='\n' || (src[0]=='\r' && src[1]=='\n')) {
    return NULL;
  }

  if(tp->closings[tp->closingsInd]==src[0]) {
    return NULL;
  }

  //
  tp->errMsg=NULL;

  // if(src[0]=='\\' && src[1]!='\0') {
  //   return src+2;
  // }

  // if(src[0]=='\\')  {
  //   if(src[1]=='\r' && src[2]=='\n') {
  //     return src+3;
  //   } else if(src[1]!='\0') {
  //     return src+2;
  //   }
  // }

  return src+1;
}

const char *parse_qchar(const char *src,const char **name,void *data) {
  struct tcl_parser *tp=(struct tcl_parser*)data;
  *name="qchar";

  if(src[0]=='"') {
    return NULL;
  }

  //
  tp->errMsg=NULL;

  if(src[0]=='\\' && src[1]=='"') {
    return src+2;
  }

  return src+1;
}

const char *parse_bchar(const char *src,const char **name,void *data) {
  struct tcl_parser *tp=(struct tcl_parser*)data;
  *name="bchar";

  if(src[0]=='}') {
    return NULL;
  }

  //
  tp->errMsg=NULL;

  // if(src[0]=='\\' && src[1]=='}') {
  //   return src+2;
  // }

  return src+1;
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

const char *parse_idn(const char *src,const char **name,void *data) {
  struct tcl_parser *tp=(struct tcl_parser*)data;
  *name="idn";

  if(src[0]=='_' ||
     (src[0]>='a' && src[0]<='z') ||
     (src[0]>='A' && src[0]<='Z') ||
     (src[0]>='0' && src[0]<='9')) {
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
void vstr_machine(struct parmac *p,const char *src);
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

    {&state_spc, &state_spc,  parse_spc, NULL},
    {&state_spc, &state_sep,  parse_sep, NULL},
    {&state_spc, &state_eol,  parse_eol, NULL},
    {&state_spc, &state_word, NULL,      word_machine},
    {&state_spc, &state_end,  NULL,      NULL},

    {&state_sep, &state_cmnt, parse_cmnt, NULL},
    {&state_sep, &state_spc,  parse_spc,  NULL},
    {&state_sep, &state_sep,  parse_sep,  NULL},
    {&state_sep, &state_eol,  parse_eol,  NULL},
    {&state_sep, &state_word, NULL,       word_machine},
    {&state_sep, &state_end,  NULL,       NULL},

    {&state_eol, &state_cmnt, parse_cmnt, NULL},
    {&state_eol, &state_spc,  parse_spc,  NULL},
    {&state_eol, &state_sep,  parse_sep,  NULL},
    {&state_eol, &state_eol,  parse_eol,  NULL},
    {&state_eol, &state_word, NULL,       word_machine},
    {&state_eol, &state_end,  NULL,       NULL}
  };

  parmac_set(p,"main",src,&state_start,&state_end,trsns, endof(trsns));
}

void word_machine(struct parmac *p,const char *src) {
  static const struct parmac_state
    state_start={"start",NULL,NULL},
    state_bstr={"bstr",bstr_enter,sub_str_leave},
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
    state_char={"char",NULL,NULL},
    state_bstr={"bstr",NULL,NULL},
    state_end={"end",NULL,NULL};

  static const struct parmac_transition trsns[]={
    {&state_start, &state_lbrace, parse_lbrace, NULL},

    {&state_lbrace, &state_rbrace, parse_rbrace, NULL},
    {&state_lbrace, &state_bstr,   NULL,         bstr_machine},
    {&state_lbrace, &state_char,   parse_bchar,  NULL},

    {&state_char, &state_rbrace, parse_rbrace, NULL},
    {&state_char, &state_bstr,   NULL,         bstr_machine},
    {&state_char, &state_char,   parse_bchar,  NULL},

    {&state_bstr, &state_rbrace, parse_rbrace, NULL},
    {&state_bstr, &state_bstr,   NULL,         bstr_machine},
    {&state_bstr, &state_char,   parse_bchar,  NULL},

    {&state_rbrace, &state_end, NULL, NULL},
  };

  parmac_set(p,"bstr",src,&state_start,&state_end,trsns,endof(trsns));
}

void qstr_machine(struct parmac *p,const char *src) {
  static const struct parmac_state
    state_start={"start",NULL,NULL},
    state_lquote={"lquote",NULL,NULL},
    state_rquote={"rquote",NULL,NULL},
    state_char={"char",char_enter,sub_str_leave},

    state_dollar={"dollar",char_enter,NULL},
    state_dchar={"dchar",NULL,sub_str_leave},
    state_vstr={"vstr",NULL,NULL},

    state_cmd={"cmd",NULL,NULL},
    state_end={"end",NULL,NULL};

  static const struct parmac_transition trsns[]={
    {&state_start, &state_lquote, parse_lquote, NULL},

    {&state_lquote, &state_rquote, parse_rquote, NULL},
    {&state_lquote, &state_dollar, parse_dollar, NULL},
    {&state_lquote, &state_cmd,    NULL,         cmd_machine},
    {&state_lquote, &state_char,   parse_qchar,  NULL},

    {&state_dollar, &state_vstr,  NULL, vstr_machine},
    {&state_dollar, &state_dchar, NULL, NULL},

    {&state_dchar, &state_rquote, parse_rquote, NULL},
    {&state_dchar, &state_dollar, parse_dollar, NULL},
    {&state_dchar, &state_cmd,    NULL,         cmd_machine},
    {&state_dchar, &state_char,   parse_qchar,  NULL},

    {&state_vstr, &state_rquote, parse_rquote, NULL},
    {&state_vstr, &state_dollar, parse_dollar, NULL},
    {&state_vstr, &state_cmd,    NULL,         cmd_machine},
    {&state_vstr, &state_char,   parse_qchar,  NULL},

    {&state_cmd, &state_rquote, parse_rquote, NULL},
    {&state_cmd, &state_dollar, parse_dollar, NULL},
    {&state_cmd, &state_cmd,    NULL,         cmd_machine},
    {&state_cmd, &state_char,   parse_qchar,  NULL},

    {&state_char, &state_rquote, parse_rquote, NULL},
    {&state_char, &state_dollar, parse_dollar, NULL},
    {&state_char, &state_cmd,    NULL,         cmd_machine},
    {&state_char, &state_char,   parse_qchar,  NULL},

    {&state_rquote, &state_end, NULL, NULL}
  };

  parmac_set(p,"qstr",src,&state_start,&state_end,trsns, endof(trsns));
}

void sstr_machine(struct parmac *p,const char *src) {
  static const struct parmac_state
    state_start={"start",NULL,NULL},
    state_char={"char",char_enter,sub_str_leave},
    state_dollar={"dollar",char_enter,NULL},
    state_dchar={"dchar",NULL,sub_str_leave},
    state_vstr={"vstr",NULL,NULL},
    state_cmd={"cmd",NULL,NULL},
    state_end={"end",NULL,NULL};

  static const struct parmac_transition trsns[]={
    {&state_start, &state_dollar, parse_dollar, NULL},
    {&state_start, &state_cmd,    NULL,         cmd_machine},
    {&state_start, &state_char,   parse_schar,  NULL},

    {&state_dollar, &state_vstr,  NULL, vstr_machine},
    {&state_dollar, &state_dchar, NULL, NULL},

    {&state_dchar, &state_dollar, parse_dollar, NULL},
    {&state_dchar, &state_cmd,    NULL, cmd_machine},
    {&state_dchar, &state_char,   parse_schar, NULL},
    {&state_dchar, &state_end,    NULL, NULL},

    {&state_vstr, &state_dollar, parse_dollar, NULL},
    {&state_vstr, &state_cmd,    NULL,         cmd_machine},
    {&state_vstr, &state_char,   parse_schar,    NULL},
    {&state_vstr, &state_end,    NULL,         NULL},

    {&state_cmd, &state_dollar, parse_dollar,NULL},
    {&state_cmd, &state_cmd,    NULL,        cmd_machine},
    {&state_cmd, &state_char,   parse_schar, NULL},
    {&state_cmd, &state_end,    NULL,        NULL},

    {&state_char, &state_dollar, parse_dollar, NULL},
    {&state_char, &state_cmd,    NULL,         cmd_machine},
    {&state_char, &state_char,   parse_schar,  NULL},
    {&state_char, &state_end,    NULL,         NULL}
  };

  parmac_set(p,"sstr",src,&state_start,&state_end,trsns,endof(trsns));
}

void vstr_machine(struct parmac *p,const char *src) {
  static const struct parmac_state
    state_start={"start",NULL,NULL},
    state_idn={"idn",char_enter,NULL},
    state_bstr={"bstr",bstr_enter,NULL},
    state_end={"end",NULL,var_leave};

  static const struct parmac_transition trsns[]={
    {&state_start, &state_bstr, NULL,      bstr_machine},
    {&state_start, &state_idn,  parse_idn, NULL},
    {&state_bstr,  &state_end,  NULL,      NULL},
    {&state_idn,   &state_idn,  parse_idn, NULL},
    {&state_idn,   &state_end,  NULL,      NULL}
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
    {&state_lsqr, &state_rsqr,  parse_rsqr, NULL},
    {&state_lsqr, &state_main,  NULL,       main_machine},
    {&state_main, &state_rsqr,  parse_rsqr, NULL},
    {&state_rsqr, &state_end,   NULL,       NULL}
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


  tcl_parser_main_machine(tp->stk,src);

  while(parmac_run(tp->stk,&stkDepth,tp,&err)) {
    if(stkDepth+1==tp->stkNum) {
      tp->stkNum*=2;
      tp->stk=(struct parmac*)realloc(tp->stk,sizeof(struct parmac)*tp->stkNum);
      tp->closings=(char*)realloc(tp->closings,tp->stkNum);
    }
  }

  {
    // const char * str="bla";
    // tcl_syntax_push(&tp->syntax,
    //                 &tp->syntaxNext,
    //                 &tp->syntaxNum,
    //                 tcl_syntax_str,
    //                 5,
    //                 str,
    //                 str+3);
  }

  printf("\n");

  unsigned int i;

  for(i=0;i<tp->syntaxNext;i++) {
    struct tcl_syntax *cur=&tp->syntax[i];
    printf("%u : ",cur->depth);

    if(cur->type==tcl_syntax_str) {
      printf("str '%s'",cur->str);
    } else if(cur->type==tcl_syntax_spc) {
      printf("spc");
    } else if(cur->type==tcl_syntax_sep) {
      printf("sep");
    }

    printf("\n");
  }

  printf("\n");

  // if(err) {
  //   printf("Error.\n");

    if(tp->errMsg!=NULL) {
      printf(tp->errMsg);
    }
  // }

  //
  // printSyntax(tp.rootStmt,0);



  printf("done %i.\n", stkDepth);

}
