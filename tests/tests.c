#include "../parmac.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

#define endof(x) (x+sizeof(x)/sizeof(*x))

struct TestData {
  char data[2048];
  unsigned int dataPos;
  const char *answers;
};

void test_enter(const char *srcStart,const char *srcEnd,bool dif,
                const char **markStart,const char **markEnd,
                void *data) {
  struct TestData *t=(struct TestData*)data;



  if(dif) {
    *markStart=srcStart;
  //   //   printf("str '");
  }

  *markEnd=srcEnd;

  // printf("%.*s",srcEnd-srcStart,srcStart);
}

void test_leave(const char *markStart,const char *markEnd, bool dif,
                void *data) {
  struct TestData *t=(struct TestData*)data;

  if(dif) {
    printf("sub_str '%.*s'\n",markEnd-markStart,markStart);
  }
}

const char *parse_a(const char *src,bool *err,const char **name,void *data) {
  *name="a";

  if(src[0]=='a') {
    return src+1;
  }

  *err=true;
  return 0;
}

const char *parse_b(const char *src,bool *err,const char **name,void *data) {
  *name="b";

  if(src[0]=='b') {
    return src+1;
  }

  *err=true;
  return 0;
}

const char *parse_c(const char *src,bool *err,const char **name,void *data) {
  *name="c";

  if(src[0]=='c') {
    return src+1;
  }

  *err=true;
  return 0;
}

const char *parse_d(const char *src,bool *err,const char **name,void *data) {
  *name="d";

  if(src[0]=='d') {
    return src+1;
  }

  *err=true;
  return 0;
}

const char *parse_x(const char *src,bool *err,const char **name,void *data) {
  *name="x";

  if(src[0]=='x') {
    return src+1;
  }

  *err=true;
  return 0;
}

void foo_machine(struct parmac *p,const char *src) {
  static const struct parmac_state
    state_start={"start",NULL,NULL},
    state_d={"d",NULL,NULL},
    state_end={"end",NULL,NULL};

  static const struct parmac_transition trsns[]={
    {&state_start, &state_d, parse_d,  NULL},
    {&state_d, &state_end, NULL,  NULL}
  };

  parmac_set(p,"foo",src,&state_start,&state_end,trsns, endof(trsns));
}

void sub_machine(struct parmac *p,const char *src) {
  static const struct parmac_state
    state_start={"start",NULL,NULL},
    state_c={"c",NULL,NULL},
    state_foo={"foo",NULL,NULL},
    state_end={"end",NULL,NULL};

  static const struct parmac_transition trsns[]={
    {&state_start, &state_c, parse_c,  NULL},
    {&state_start, &state_foo, NULL,  foo_machine},
    {&state_c, &state_end, NULL,  NULL},
    {&state_foo, &state_end, NULL,  NULL}
  };

  parmac_set(p,"sub",src,&state_start,&state_end,trsns, endof(trsns));
}

void main_machine(struct parmac *p,const char *src) {
  static const struct parmac_state
    state_start={"start",NULL,NULL},
    state_a={"a",NULL,NULL},
    state_b={"b",NULL,NULL},
    state_sub={"sub",NULL,NULL},
    state_x={"x",NULL,NULL},
    state_end={"end",NULL,NULL};

  static const struct parmac_transition trsns[]={
    {&state_start, &state_a,   parse_a,  NULL},
    {&state_a, &state_b,   parse_b,  NULL},
    {&state_b, &state_sub,   NULL,  sub_machine},
    {&state_b, &state_b,   parse_b,  NULL},
    {&state_b, &state_x,   parse_x,  NULL},
    {&state_sub, &state_x, parse_x,  NULL},
    {&state_sub, &state_end, NULL,  NULL},
    {&state_x, &state_end, NULL,  NULL}
  };

  parmac_set(p,"main",src,&state_start,&state_end,trsns, endof(trsns));
}

void test_B040() {
}


int main() {

  struct parmac stk[2048];
  struct parmac *p=stk;
  const char *str="abdx";
  main_machine(p,str);

  bool err;
  char errMsg[2048];
  errMsg[0]='\0';

  struct TestData t;

  while(p=parmac_run(p,&t,&err,errMsg,sizeof(errMsg),false)) {

  }

  if(err) {
    printf("\n-----------\nErr\n");
    printf("'%s'\n",errMsg);

  }

  printf("done\n");
  // printf("done %i\n",(p-1)->src==endof(str));
  // printf("'%s'\n",p->src);
  return 0;

}
