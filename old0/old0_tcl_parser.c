#include "old0_tcl_parser.h"

#include <stdio.h>
#include <stddef.h>

#define endof(x) (x+sizeof(x)/sizeof(*x))

#define parse_sep tcl_parser_parse_sep
#define parse_eol tcl_parser_parse_eol
#define parse_spc tcl_parser_parse_spc
#define parse_any tcl_parser_parse_any
#define parse_hash tcl_parser_parse_hash

#define parse_lquote tcl_parser_parse_lquote
#define parse_rquote tcl_parser_parse_rquote
#define parse_lsqr tcl_parser_parse_lsqr
#define parse_rsqr tcl_parser_parse_rsqr
#define parse_lbrace tcl_parser_parse_lbrace
#define parse_rbrace tcl_parser_parse_rbrace

#define parse_dollar tcl_parser_parse_dollar
#define parse_idn tcl_parser_parse_idn

#define main_machine tcl_parser_main_machine
#define word_machine tcl_parser_word_machine
#define bstr_machine tcl_parser_bstr_machine
#define qstr_machine tcl_parser_qstr_machine
#define sstr_machine tcl_parser_sstr_machine
#define cmd_machine tcl_parser_cmd_machine
#define vstr_machine tcl_parser_vstr_machine



void char_enter(const struct parmac_state *fromState,
                const struct parmac_state *toState,
                const char *srcStart,const char *srcEnd,
                void *data) {

}

void sub_str_leave(const struct parmac_state *fromState,
                const struct parmac_state *toState,
                void *data) {

}

void var_leave(const struct parmac_state *fromState,
                const struct parmac_state *toState,
                void *data) {
}

void word_leave(const struct parmac_state *fromState,
                const struct parmac_state *toState,
                void *data) {
}

void stmt_leave(const struct parmac_state *fromState,
                const struct parmac_state *toState,
                void *data) {
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

const char *parse_any(const char *src,const char **name,void *data) {
  struct tcl_parser *tp=(struct tcl_parser*)data;
  *name="any";

  if(src[0]!='\0') {
    tp->errMsg=NULL;
    return src+1;
  }

  return NULL;
}

const char *parse_hash(const char *src,const char **name,void *data) {
  struct tcl_parser *tp=(struct tcl_parser*)data;
  *name="hash";

  if(src[0]=='#') {
    tp->errMsg=NULL;
    return src+1;
  }

  return NULL;
}

const char *parse_lquote(const char *src,const char **name,void *data) {
  struct tcl_parser *tp=(struct tcl_parser*)data;
  *name="lquote";

  if(src[0]=='"') {
    tp->errMsg=NULL;
    return src+1;
  }

  return NULL;
}

const char *parse_rquote(const char *src,const char **name,void *data) {
  struct tcl_parser *tp=(struct tcl_parser*)data;
  *name="rquote";

  if(src[0]=='"') {
    tp->errMsg=NULL;
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
    return src+1;
  }

  return NULL;
}

const char *parse_rsqr(const char *src,const char **name,void *data) {
  struct tcl_parser *tp=(struct tcl_parser*)data;
  *name="rsqr";

  if(src[0]==']') {
    tp->errMsg=NULL;
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
    return src+1;
  }

  return NULL;
}

const char *parse_rbrace(const char *src,const char **name,void *data) {
  struct tcl_parser *tp=(struct tcl_parser*)data;
  *name="rbrace";

  if(src[0]=='}') {
    tp->errMsg=NULL;
    return src+1;
  }

  tp->errMsg="Expecting closing curly brace.\n";
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
void word_machine(struct parmac *p,const char *src);
void bstr_machine(struct parmac *p,const char *src);
void qstr_machine(struct parmac *p,const char *src);
void sstr_machine(struct parmac *p,const char *src);
void vstr_machine(struct parmac *p,const char *src);
void cmd_machine(struct parmac *p,const char *src);

void main_machine(struct parmac *p,const char *src) {
  static const struct parmac_state
    state_start={"start",NULL,NULL},
    state_cmnt_head={"cmnt_head",NULL,NULL},
    state_cmnt_tail={"cmnt_tail",NULL,NULL},
    state_word1={"word1",NULL,word_leave},
    state_wordn={"wordn",NULL,word_leave},
    state_spc1={"spc1",NULL,NULL},
    state_spcn={"spcn",NULL,NULL},
    state_sep={"sep",NULL,stmt_leave},
    state_eol={"eol",NULL,stmt_leave},
    state_end={"end",NULL,stmt_leave};

  static const struct parmac_transition trsns[]={
    {&state_start, &state_cmnt_head, parse_hash, NULL},
    {&state_start, &state_sep,   parse_sep,  NULL},
    {&state_start, &state_eol,   parse_eol,  NULL},
    {&state_start, &state_spc1,  parse_spc,  NULL},
    {&state_start, &state_end,   NULL,       NULL},
    {&state_start, &state_word1, NULL,       word_machine},

    {&state_cmnt_head, &state_eol,   parse_eol, NULL},
    {&state_cmnt_head, &state_cmnt_tail, parse_any, NULL},
    {&state_cmnt_head, &state_end,   NULL,      NULL},

    {&state_cmnt_tail, &state_eol,   parse_eol, NULL},
    {&state_cmnt_tail, &state_cmnt_tail, parse_any, NULL},
    {&state_cmnt_tail, &state_end,   NULL,      NULL},

    {&state_word1, &state_spcn, parse_spc, NULL},//disable to test excurs 'go to root exit'
    {&state_word1, &state_sep,  parse_sep, NULL},
    {&state_word1, &state_eol,  parse_eol, NULL},
    {&state_word1, &state_end,  NULL,      NULL},

    {&state_wordn, &state_spcn, parse_spc, NULL},
    {&state_wordn, &state_sep,  parse_sep, NULL},
    {&state_wordn, &state_eol,  parse_eol, NULL},
    {&state_wordn, &state_end,  NULL,      NULL},

    {&state_spc1, &state_cmnt_head, parse_hash, NULL},
    {&state_spc1, &state_spc1,  parse_spc,  NULL},
    {&state_spc1, &state_sep,   parse_sep,  NULL},
    {&state_spc1, &state_eol,   parse_eol,  NULL},
    {&state_spc1, &state_end,   NULL,       NULL},
    {&state_spc1, &state_word1, NULL,       word_machine},

    {&state_spcn, &state_spcn,  parse_spc, NULL},
    {&state_spcn, &state_sep,   parse_sep, NULL},
    {&state_spcn, &state_eol,   parse_eol, NULL},
    {&state_spcn, &state_end,   NULL,      NULL},
    {&state_spcn, &state_wordn, NULL,      word_machine},

    {&state_sep, &state_cmnt_head, parse_hash, NULL},
    {&state_sep, &state_spc1,  parse_spc,  NULL},
    {&state_sep, &state_sep,   parse_sep,  NULL},
    {&state_sep, &state_eol,   parse_eol,  NULL},
    {&state_sep, &state_end,   NULL,       NULL},
    {&state_sep, &state_word1, NULL,       word_machine},

    {&state_eol, &state_cmnt_head, parse_hash, NULL},
    {&state_eol, &state_spc1,  parse_spc,  NULL},
    {&state_eol, &state_sep,   parse_sep,  NULL},
    {&state_eol, &state_eol,   parse_eol,  NULL},
    {&state_eol, &state_end,   NULL,       NULL},
    {&state_eol, &state_word1, NULL,       word_machine}
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
    {&state_start, &state_lbrace, parse_lbrace,  NULL},

    {&state_lbrace, &state_rbrace, parse_rbrace, NULL},
    {&state_lbrace, &state_bstr,   NULL,         bstr_machine},
    {&state_lbrace, &state_char,   parse_any,    NULL},

    {&state_char, &state_rbrace, parse_rbrace, NULL},
    {&state_char, &state_bstr,   NULL,         bstr_machine},
    {&state_char, &state_char,   parse_any,    NULL},

    {&state_bstr, &state_rbrace, parse_rbrace, NULL},
    {&state_bstr, &state_bstr,   NULL,         bstr_machine},
    {&state_bstr, &state_char,   parse_any,    NULL},

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
    {&state_lquote, &state_char,   parse_any,    NULL},

    {&state_dollar, &state_vstr,  NULL, vstr_machine},
    {&state_dollar, &state_dchar, NULL, NULL},

    {&state_dchar, &state_rquote, parse_rquote, NULL},
    {&state_dchar, &state_dollar, parse_dollar, NULL},
    {&state_dchar, &state_cmd,    NULL, cmd_machine},
    {&state_dchar, &state_char,   parse_any, NULL},

    {&state_vstr, &state_rquote, parse_rquote, NULL},
    {&state_vstr, &state_dollar, parse_dollar, NULL},
    {&state_vstr, &state_cmd,    NULL,         cmd_machine},
    {&state_vstr, &state_char,   parse_any,  NULL},

    {&state_cmd, &state_rquote, parse_rquote, NULL},
    {&state_cmd, &state_dollar, parse_dollar, NULL},
    {&state_cmd, &state_cmd,    NULL,         cmd_machine},
    {&state_cmd, &state_char,   parse_any,    NULL},

    {&state_char, &state_rquote, parse_rquote, NULL},
    {&state_char, &state_dollar, parse_dollar, NULL},
    {&state_char, &state_cmd,    NULL,         cmd_machine},
    {&state_char, &state_char,   parse_any,    NULL},

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
    {&state_start, &state_char,   parse_any,  NULL},

    {&state_dollar, &state_vstr,  NULL, vstr_machine},
    {&state_dollar, &state_dchar, NULL, NULL},

    {&state_dchar, &state_dollar, parse_dollar, NULL},
    {&state_dchar, &state_cmd,    NULL, cmd_machine},
    {&state_dchar, &state_end,    NULL, NULL},
    {&state_dchar, &state_char,   parse_any, NULL},

    {&state_vstr, &state_dollar, parse_dollar, NULL},
    {&state_vstr, &state_cmd,    NULL,         cmd_machine},
    {&state_vstr, &state_end,    NULL,         NULL},
    {&state_vstr, &state_char,   parse_any,  NULL},

    {&state_cmd, &state_dollar, parse_dollar,NULL},
    {&state_cmd, &state_cmd,    NULL,        cmd_machine},
    {&state_cmd, &state_end,    NULL,        NULL},
    {&state_cmd, &state_char,   parse_any, NULL},

    {&state_char, &state_dollar, parse_dollar, NULL},
    {&state_char, &state_cmd,    NULL,         cmd_machine},
    {&state_char, &state_end,    NULL,         NULL},
    {&state_char, &state_char,   parse_any,  NULL}
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

    {&state_bstr, &state_end, NULL, NULL},

    {&state_idn, &state_idn, parse_idn, NULL},
    {&state_idn, &state_end, NULL,      NULL}
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

    {&state_lsqr, &state_rsqr, parse_rsqr, NULL},
    {&state_lsqr, &state_main, NULL,       main_machine},

    {&state_main, &state_rsqr, parse_rsqr, NULL},

    {&state_rsqr, &state_end, NULL, NULL}
  };

  parmac_set(p,"cmd",src,&state_start,&state_end,trsns, endof(trsns));
}
