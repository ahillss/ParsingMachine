#include "old1_tcl_parser.h"

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


void push_str(struct tcl_parser *tp) {
  size_t strLen=tp->markEnd-tp->markStart;
  struct tcl_syntax_sub *sub;

  sub=(struct tcl_syntax_sub*)malloc(sizeof(struct tcl_syntax_sub));

  sub->type=tcl_syntax_sub_str;
  sub->valStmt=NULL;
  sub->next=NULL;
  sub->valStr=(char*)malloc(strLen+1);

  memcpy(sub->valStr,tp->markStart,strLen);
  sub->valStr[strLen]='\0';

  //
  struct tcl_syntax_build *build;
  build=tp->buildStk;

  if(build->firstSub) {
    build->lastSub->next=sub;
  } else {
    build->firstSub=sub;
  }

  build->lastSub=sub;
}

void push_var(struct tcl_parser *tp) {
  size_t strLen=tp->markEnd-tp->markStart;
  struct tcl_syntax_sub *sub;

  sub=(struct tcl_syntax_sub*)malloc(sizeof(struct tcl_syntax_sub));

  sub->type=tcl_syntax_sub_var;
  sub->valStmt=NULL;
  sub->next=NULL;
  sub->valStr=(char*)malloc(strLen+1);

  memcpy(sub->valStr,tp->markStart,strLen);
  sub->valStr[strLen]='\0';

  // if(tp->firstSub) {
  //   tp->lastSub->next=sub;
  // } else {
  //   tp->firstSub=sub;
  // }

  // tp->lastSub=sub;
}

void push_word(struct tcl_parser *tp) {
  struct tcl_syntax_word *word;

  word=(struct tcl_syntax_word*)malloc(sizeof(struct tcl_syntax_word));

  // word->subs=tp->firstSub;
  word->next=NULL;

  // tp->firstSub=NULL;
  // tp->lastSub=NULL;

  // if(tp->firstWord) {
  //   tp->lastWord->next=word;
  // } else {
  //   tp->firstWord=word;
  // }

  // tp->lastWord=word;
}

void push_stmt(struct tcl_parser *tp) {

  // if(tp->firstWord) {
    struct tcl_syntax_stmt *stmt;

    stmt=(struct tcl_syntax_stmt*)malloc(sizeof(struct tcl_syntax_stmt));

    // stmt->words=tp->firstWord;
    stmt->next=NULL;

  //   tp->firstWord=NULL;
  //   tp->lastWord=NULL;

  //   if(tp->firstStmt) {
  //     tp->lastStmt->next=stmt;
  //   } else {
  //     tp->firstStmt=stmt;
  //   }

  //   tp->lastStmt=stmt;
  // }
}

void push_build(struct tcl_parser *tp) {

  tp->recurseDepth++;

  struct tcl_syntax_build *build;
  build=(struct tcl_syntax_build*)malloc(sizeof(struct tcl_syntax_build));

  build->firstStmt=NULL;
  build->lastStmt=NULL;
  build->firstWord=NULL;
  build->lastWord=NULL;
  build->firstSub=NULL;
  build->lastSub=NULL;
  build->next=tp->buildStk;
}

void pop_build(struct tcl_parser *tp) {
  tp->recurseDepth--;

  // if(tp->buildStk->next) {
  //   tp->buildStk->next->lastSub->valStmt=tp->buildStk->firstStmt;
  // } else {
  //   tp->rootStmt=tp->buildStk->firstStmt;
  // }

  // struct tcl_syntax_build *tmp;
  // tmp=tp->buildStk;
  // tp->buildStk=tmp->next;
  // free(tmp);
}

////////////////////////////////////

void char_enter(const struct parmac_state *fromState,
                const struct parmac_state *toState,
                const char *srcStart,const char *srcEnd,
                void *data) {


  struct tcl_parser *tp=(struct tcl_parser*)data;

  if(fromState!=toState) {
    tp->markStart=srcStart;
  }

  tp->markEnd=srcEnd;
}

void sub_str_leave(const struct parmac_state *fromState,
                   const struct parmac_state *toState,
                   void *data) {
  struct tcl_parser *tp=(struct tcl_parser*)data;

  if(fromState!=toState) {
    printDepth(tp->recurseDepth+1);

    printf("sub_str '%.*s'\n",(int)(tp->markEnd - tp->markStart),tp->markStart);

  }
}

void var_leave(const struct parmac_state *fromState,
               const struct parmac_state *toState,
               void *data) {
  struct tcl_parser *tp=(struct tcl_parser*)data;
    printDepth(tp->recurseDepth+1);
  printf("var '%.*s'\n",(int)(tp->markEnd-tp->markStart),tp->markStart);

}

void word_leave(const struct parmac_state *fromState,
                const struct parmac_state *toState,
                void *data) {
  struct tcl_parser *tp=(struct tcl_parser*)data;
  printf(":");
  printDepth(tp->recurseDepth-1);
  printf("word\n");


}

void stmt_leave(const struct parmac_state *fromState,
                const struct parmac_state *toState,
                void *data) {
  struct tcl_parser *tp=(struct tcl_parser*)data;
  printf(":");
  printDepth(tp->recurseDepth-1);
  printf("stmt\n");


}



void main_enter(const struct parmac_state *fromState,
                const struct parmac_state *toState,
                const char *srcStart,const char *srcEnd,
                void *data) {


  struct tcl_parser *tp=(struct tcl_parser*)data;
  push_build(tp);
}

void main_leave(const struct parmac_state *fromState,
                const struct parmac_state *toState,
                void *data) {

  stmt_leave(fromState,toState,data);

  struct tcl_parser *tp=(struct tcl_parser*)data;


}

///////////////////////////////////////////////

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

const char *parse_esc_eol(const char *src,const char **name,void *data) {
  struct tcl_parser *tp=(struct tcl_parser*)data;
  *name="spc";

  if(src[0]=='\\' || src[1]=='\n') {
    tp->errMsg=NULL;
    return src+2;
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
    tp->closingIt=(tp->closingIt)?tp->closingIt+1:tp->closingStart;
    *(tp->closingIt)='"';
    return src+1;
  }

  return NULL;
}


const char *parse_rquote(const char *src,const char **name,void *data) {
  struct tcl_parser *tp=(struct tcl_parser*)data;
  *name="rquote";

  if(src[0]=='"') {
    tp->closingIt=(tp->closingIt==tp->closingStart)?NULL:tp->closingIt-1;
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
    tp->closingIt=(tp->closingIt)?tp->closingIt+1:tp->closingStart;
    *(tp->closingIt)=']';
    return src+1;
  }

  return NULL;
}

const char *parse_rsqr(const char *src,const char **name,void *data) {
  struct tcl_parser *tp=(struct tcl_parser*)data;
  *name="rsqr";

  if(src[0]==']') {
    tp->closingIt=(tp->closingIt==tp->closingStart)?NULL:tp->closingIt-1;
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
    tp->closingIt=(tp->closingIt)?tp->closingIt+1:tp->closingStart;
    *(tp->closingIt)='}';
    return src+1;
  }

  return NULL;
}

const char *parse_rbrace(const char *src,const char **name,void *data) {
  struct tcl_parser *tp=(struct tcl_parser*)data;
  *name="rbrace";

  if(src[0]=='}') {
    tp->closingIt=(tp->closingIt==tp->closingStart)?NULL:tp->closingIt-1;
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

  if(tp->closingIt && tp->closingIt[0]==src[0]) {
    return NULL;
  }

  tp->errMsg=NULL;
  return src+1;
}

const char *parse_qchar(const char *src,const char **name,void *data) {
  struct tcl_parser *tp=(struct tcl_parser*)data;
  *name="qchar";

  if(src[0]=='"') {
    return NULL;
  }

  tp->errMsg=NULL;
  return src+1;
}

const char *parse_bchar(const char *src,const char **name,void *data) {
  struct tcl_parser *tp=(struct tcl_parser*)data;
  *name="bchar";

  if(src[0]=='}') {
    return NULL;
  }

  tp->errMsg=NULL;
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
    state_start={"start",main_enter,NULL},
    state_cmnt={"cmnt",NULL,NULL},
    state_word={"word",NULL,word_leave},
    state_spc={"spc",NULL,NULL},
    state_sep={"sep",NULL,stmt_leave},
    state_eol={"eol",NULL,stmt_leave},
    state_end={"end",NULL,main_leave};

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
    state_bstr={"bstr",NULL,sub_str_leave},
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
    state_char={"char",char_enter,NULL},
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
    state_bstr={"bstr",NULL,NULL},
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
    state_start={"start",NULL,NULL},
    state_lsqr={"lsqr",NULL,NULL},
    state_rsqr={"rsqr",NULL,NULL},
    state_main={"main",NULL,NULL},
    state_end={"end",NULL,NULL};

  static const struct parmac_transition trsns[]={
    {&state_start, &state_lsqr, parse_lsqr, NULL},
    {&state_lsqr, &state_rsqr,  parse_rsqr, NULL},
    {&state_lsqr, &state_main,  NULL,       main_machine},
    {&state_main, &state_rsqr,  parse_rsqr, NULL},
    {&state_rsqr, &state_end,   NULL,       NULL}
  };

  parmac_set(p,"cmd",src,&state_start,&state_end,trsns, endof(trsns));
}
