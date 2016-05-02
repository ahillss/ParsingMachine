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

struct ParserData {
  const char *errMsg;
  int pos,row,col;
  const char *markStart,*markEnd;
};

// void test_enter(const char *srcStart,const char *srcEnd,bool dif,void *data) {
//   struct ParserData *pd=(struct ParserData*)data;

//   if(dif) {
//     pd->markStart=srcStart;
//   }

//   pd->markEnd=srcEnd;

//   //printf(" : enter '%.*s'\n",srcEnd-srcStart,srcStart);
//   // printf("- %s_%s_enter(%i) '%.*s'\n","p->name", state->name,dif, srcEnd-srcStart,srcStart);
// }

// void test_leave(bool dif,void *data) {
//   struct ParserData *pd=(struct ParserData*)data;

//   if(dif) {
//   }
//   //printf(" : leave '%.*s'\n",pd->markEnd-pd->markStart,pd->markStart);

//   // printf("- %s_%s_leave(%i) '%.*s'\n","p->name", state->name,dif, pd->markEnd-pd->markStart,pd->markStart);
// }


void test_enter(const struct parmac_state *fromState,
                const struct parmac_state *toState,
                const char *srcStart,const char *srcEnd,
                void *data) {

  struct ParserData *pd=(struct ParserData*)data;
  bool dif=fromState!=toState;

  //if(dif) {
  pd->markStart=srcStart;
  //}

  pd->markEnd=srcEnd;

  // printf("enter (%s -> %s) '%.*s'\n",
  //        fromState?fromState->name:"",toState?toState->name:"",
  //        srcEnd-srcStart,srcStart);
}

void test_leave(const struct parmac_state *fromState,
                const struct parmac_state *toState,
                void *data) {

  struct ParserData *pd=(struct ParserData*)data;
  bool dif=fromState!=toState;

  // printf("leave (%s -> %s) '%.*s'\n",
  //        fromState?fromState->name:"",toState?toState->name:"",
  //        pd->markEnd-pd->markStart,pd->markStart);
}

const char *parse_a(const char *src,const char **name,void *data) {
  *name="a";

  if(src[0]=='a') {
    return src+1;
  }

  return NULL;
}

const char *parse_b(const char *src,const char **name,void *data) {
  *name="b";

  if(src[0]=='b') {
    return src+1;
  }

  return NULL;
}

const char *parse_c(const char *src,const char **name,void *data) {
  *name="c";

  if(src[0]=='c') {
    return src+1;
  }

  return NULL;
}

const char *parse_d(const char *src,const char **name,void *data) {
  *name="d";

  if(src[0]=='d') {
    return src+1;
  }

  return NULL;
}

const char *parse_x(const char *src,const char **name,void *data) {
  *name="x";

  if(src[0]=='x') {
    return src+1;
  }

  return NULL;
}

void foo_machine(struct parmac *p,const char *src) {
  static const struct parmac_state
    state_start={"start",test_enter,test_leave},
    state_d={"d",test_enter,test_leave},
    state_end={"end",test_enter,test_leave};

  static const struct parmac_transition trsns[]={
    {&state_start, &state_d, parse_d,  NULL},
    {&state_d, &state_end, NULL,  NULL}
  };

  parmac_set(p,"foo",src,&state_start,&state_end,trsns, endof(trsns));
}

void sub_machine(struct parmac *p,const char *src) {
  static const struct parmac_state
    state_start={"start",test_enter,test_leave},
    state_c={"c",test_enter,test_leave},
    state_foo={"foo",test_enter,test_leave},
    state_end={"end",test_enter,test_leave};

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
    state_start={"start",test_enter,test_leave},
    state_a={"a",test_enter,test_leave},
    state_b={"b",test_enter,test_leave},
    state_sub={"sub",test_enter,test_leave},
    state_x={"x",test_enter,test_leave},
    state_end={"end",test_enter,test_leave};

  static const struct parmac_transition trsns[]={
    {&state_start, &state_a,   parse_a,  NULL},
    {&state_a, &state_b,   parse_b,  NULL},
    {&state_b, &state_sub,   NULL,  sub_machine},
    {&state_b, &state_b,   parse_b,  NULL},
    {&state_b, &state_x,   parse_x,  NULL},
    {&state_sub, &state_b, parse_b,  NULL},
    {&state_sub, &state_x, parse_x,  NULL},
    {&state_sub, &state_end, NULL,  NULL},
    {&state_x, &state_end, NULL,  NULL}
  };

  parmac_set(p,"main",src,&state_start,&state_end,trsns, endof(trsns));
}

void test_B040(struct parmac *p,struct TestData *t) {
  printf("testing B040\n");


}


int main() {
  bool err;
  struct parmac stk[2048];
  struct parmac *p=stk;
  const char *src="abbbc";
  main_machine(p,src);

  struct ParserData d;
  d.errMsg=NULL;
  d.pos=d.row=d.col=0;
  d.markStart=src;
  d.markEnd=src;

  while(parmac_run(&p,&d,&err,false)) {

  }

  if(err) {
    printf("\n-----------\nErr\n");

    if(d.errMsg) {
      printf("Expecting %s\n",d.errMsg);
    }

    printf("at %i\n",(int)(p->src-src));
  }



  printf("done\n");

  return 0;

}
